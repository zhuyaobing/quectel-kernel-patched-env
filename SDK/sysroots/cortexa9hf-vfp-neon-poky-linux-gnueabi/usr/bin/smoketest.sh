#!/bin/bash
#
# author: arwi
#

BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
NF='\033[0m'

PROGRAM_NAME=$(basename $0)

# All allowed variants must be listed as lowercase string.
declare -a variants=( "n2" "tbm3" "vcm" )

print_help()
{
   echo "Usage $PROGRAM_NAME: [-hvc] <variant>"
   echo "  -h       Print help"
   echo "  -v       Show verbose printouts (systemctl status output etc.)"
   echo "  -c       Include systemd-analyze critical-chain in output!"
   echo
   echo "  <variant>   Currently supported variants are:"
   printf '\t%s\n' "${variants[@]}"
}

verbose=0
with_critchains=0

declare -a app_version_parameters=( "mosquitto:-h" )
declare -a app_version_line=( "maintenance_app:2" )

################################################################################

banner()
{
   printf "\n================================================================================\n"
   printf "$@\n"
   printf "================================================================================\n\n"
}

################################################################################

print_colorful()
{
   local color=$1
   printf "$color$2$NF"
}

################################################################################

print_if_verbose()
{
   if [ $verbose = 1 ]; then
      echo "$@"
   fi
}
################################################################################

get_app_version()
{
   local app=$1
   local tempout=$(mktemp)

   local vp="--version"

   for appparam in $app_version_parameters; do
      if [[ "${appparam}" == "${app}:"* ]]; then
         vp=${appparam#*:}
      fi
   done
   local out_line=""
   for appparam in $app_version_line; do
      if [[ "${appparam}" == "${app}:"* ]]; then
         out_line=${appparam#*:}
      fi
   done

   if [ -n "$out_line" ]; then
      $app "$vp" | sed "${out_line}q;d" > "${tempout}" 2>&1 &
   else
      $app "$vp" > "${tempout}" 2>&1 &
   fi

   local pid=$!
   disown
   usleep 500000
   kill -9 "$pid" > /dev/null 2>&1
   {
      read version
   } < "$tempout"
   rm -f "$tempout" 2> /dev/null
   echo "$version"
}

################################################################################

check_e4c()
{
   local hw_variant=`ecuvarc hwvariant`
   local evd="evd"
   local should_run=false

   if [[ "$hw_variant" == *"VCM_2_BV3"* ]]; then
      should_run=true;
   fi

   printf "${BOLD}%-25s${NF}" "$evd"

   enabled_out=`systemctl is-enabled $evd 2>&1`
   enabled_status=$?
   active_out=`systemctl is-active $evd`
   active_status=$?
   out=`systemctl status $evd`
   status_status=$?

   if [ $enabled_status == 0 ]; then
         printf "${GREEN}%-10s" "ENABLED"
   else
         printf "${RED}%-10s" "DISABLED"
   fi

   if [ $active_status == 0 ]; then
      if [ "$should_run" = true ]; then
         printf "${GREEN}%-13s" "RUNNING"
      else
         printf "${GREEN}%-13s" "NOT RUNNING"
      fi
   else
      if [ "$should_run" = true ]; then
         printf "${RED}%-13s" "NOT RUNNING"
      else
         printf "${GREEN}%-13s" "NOT RUNNING"
      fi
   fi

   if [ $status_status == 0 ]; then
      if [ "$should_run" = true ]; then
         printf "${GREEN}%-8s" "OK"
      else
         printf "${RED}%-8s" "NOT OK"
      fi
   else
      if [ "$should_run" = true ]; then
         printf "${RED}%-8s" "NOT OK"
      else
         printf "${GREEN}%-8s" "OK"
      fi
   fi
   printf "${NF}\n"
}

################################################################################

check_services()
{
   local services="$@"
   local evd="evd"
   banner "Check systemd services"

   for service in $services; do

      if [[ "$service" == *"$evd"* ]]; then
         check_e4c
         continue
      fi

      printf "${BOLD}%-25s${NF}" "$service"

      enabled_out=`systemctl is-enabled $service 2>&1`
      enabled_status=$?
      enabled=""
      if [ $enabled_status == 0 ]; then
         printf "${GREEN}%-10s" "ENABLED"
      else
         printf "${RED}%-10s" "DISABLED"
      fi
      printf "${NF}"

      active_out=`systemctl is-active $service`
      active_status=$?
      active=""
      if [ $active_status == 0 ]; then
         printf "${GREEN}%-13s" "RUNNING"
      else
         printf "${RED}%-13s" "NOT RUNNING"
      fi
      printf "${NF}"


      out=`systemctl status $service`
      status_status=$?
      if [ $status_status == 0 ]; then
         printf "${GREEN}%-8s" "OK"
      else
         printf "${RED}%-8s" "NOT OK"
      fi
      printf "${NF}\n"

      sum=$((enabled_status + active_status + status_status))
      if [ $sum == 0 ];then
         print_if_verbose "$out"
         print_if_verbose "----------------------------------------------------------------"
      else
         printf "$out\n"
         echo "----------------------------------------------------------------"
      fi
   done
}

################################################################################

check_ecu_vars()
{
   local vars="serialnumber vehicleid hwversion hwvariant vehiclevariant versionuboot versionhypervisor versioncsp versiontelekernel versionteledtb versiontpakernel versionupdatekernel versionupdatedtb topversion versionlapi versionlapimeta ecustate"

   banner "Check ECU variables"

   for v in $vars; do
      printf "${BOLD}%-25s${NF}" "$v"
      space=8

      out=`ecuvarc $v`
      status=$?
      if [ $status = 0 ]; then
         if [ "$v" == "topversion" ] && [[ "$out" =~ "xxx"* ]]; then
            printf "${RED}%-${space}s" "ERROR"
         else
            printf "${GREEN}%-${space}s" "OK"
         fi
      else
         printf "${RED}%-${space}s" "ERROR"
      fi
      printf "${NF}$out\n"

   done
}

################################################################################

systemd_analyze_chain()
{
   local unit=$1

   banner "systemd analyze critical chain for: $unit"

   local out=`systemd-analyze critical-chain $unit`
   local status=$?
   if [ $status = 0 ]; then
      print_colorful $GREEN "OK"
      printf " $unit\n"
      echo "$out"
      echo "----------------------------------------------------------------"
   else
      print_colorful $RED "ERROR"
      printf " $unit: %s\n" "$out"
   fi
}

################################################################################

check_systime()
{
   banner "System time, check for 1970"

   local out=`date`
   local status=$?
   if [ $status != 0 ]; then
      print_colorful $RED "ERROR    "
      echo " date return error code, output: $out"
   elif [[ "$out" =~ "1970" ]]; then
      print_colorful $RED "ERROR    "
      echo "System time is 1970 (epoch): '$out'"
   else
      print_colorful $GREEN "OK    "
      echo "$out"
   fi
}

################################################################################

list_app_versions()
{
   local apps="$@"
   local version=""

   banner "List app versions"

   for app in $apps; do
      version=$(get_app_version "$app")
      printf "${BOLD}%-20s${NF}%s\n" "$app" "$version"
   done
}


################################################################################
list_services()
{
	local alex_services=$(systemctl show -p Wants alex*.target | grep -io '[^= ]*.service')
	declare -a should_run

	for service in $alex_services; do
		service_name=${service%.service}
		type=$(systemctl show $service --property=Type | cut -d= -f2)

		if [[ "$type" != "oneshot" && "$type" != "idle" ]]; then
			should_run+=( $service_name )
		fi
	done

	echo ${should_run[*]}
}

################################################################################
# MAIN
################################################################################

while getopts "havcV" OPT; do
   case $OPT in
      v)
         verbose=1
         print_if_verbose "Set verbose"
         ;;
      c)
         with_critchains=1
         print_if_verbose "With critical chain!"
         ;;
      a)
         print_if_verbose '"-a" (MAN apps) switch no longer has any effect.'
         ;;
      h)
         print_help
         exit 0
         ;;
      \?)
         #echo "Invalid option: -$OPTARG" >&2
         print_help
         exit 1
         ;;
      *)
         print_help
         exit 1
         ;;
   esac
done
shift $((OPTIND-1))

# Validate product variant.
# All allowed variants must be listed as lowercase string in the array "$variants".

variant="$@"
if [ -z "$variant" ]; then
   echo "No variant given, get it automatically (ecuvarc)."
   variant=`ecuvarc hwversion 2>&1`
else
   variant=`echo $variant | tr '[:upper:]' '[:lower:]'`

   match=$(printf '%s\n' "${variants[@]}" | grep -ow "${variant}")
   if [[ ! -z $match ]]; then
      echo "Variant ${variant} specified from command line."
   else
      print_colorful $RED "ERROR: "
      printf "a valid product variant must be specified!\n\n"
      print_help
      exit 1
   fi
fi

services=$( list_services )
apps=$services

banner "SMOKETEST for $variant"

if [ $with_critchains = 1 ]; then
   systemd_analyze_chain "alex-boot.target"
   systemd_analyze_chain "alex-core.target"
   systemd_analyze_chain "alex.target"
fi

list_app_versions "$apps"

check_services "$services"
check_ecu_vars
check_systime
