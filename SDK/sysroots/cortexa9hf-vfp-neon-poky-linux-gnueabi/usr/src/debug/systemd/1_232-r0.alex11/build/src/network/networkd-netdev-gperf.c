/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf  */
/* Computed positions: -k'2,5,8-9,15,20-21,23' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


#include <stddef.h>
#include "conf-parser.h"
#include "network-internal.h"
#include "networkd-netdev-bond.h"
#include "networkd-netdev-bridge.h"
#include "networkd-netdev-ipvlan.h"
#include "networkd-netdev-macvlan.h"
#include "networkd-netdev-tunnel.h"
#include "networkd-netdev-tuntap.h"
#include "networkd-netdev-veth.h"
#include "networkd-netdev-vlan.h"
#include "networkd-netdev-vxlan.h"
#include "networkd-netdev-vrf.h"
#include "networkd-netdev.h"
#include "vlan-util.h"
#include <string.h>

#define TOTAL_KEYWORDS 90
#define MIN_WORD_LENGTH 7
#define MAX_WORD_LENGTH 27
#define MIN_HASH_VALUE 8
#define MAX_HASH_VALUE 163
/* maximum key range = 156, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
network_netdev_gperf_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164,   0, 164, 164, 164,
       60,  25, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164,  35,  20,  10,   0,   0,
       10,  80,   5,  45, 164,  75,  75,   0,   0,  65,
       15,  35,  30,  20,   0,   0,  10, 164,  10, 164,
      164, 164, 164, 164, 164, 164, 164,   5, 164,  35,
       65,   0, 164,   5,  40,   0, 164,  10,  15,  20,
        0,   0,  25, 164,   0,   0,   0,   0, 164,  55,
       10,  10,  15, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
      case 18:
      case 17:
      case 16:
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
      case 13:
      case 12:
      case 11:
      case 10:
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct ConfigPerfItem *
network_netdev_gperf_lookup (register const char *str, register unsigned int len)
{
  static const struct ConfigPerfItem wordlist[] =
    {
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {"Tun.User",                    config_parse_string,                0,                             offsetof(TunTap, user_name)},
      {(char*)0},
      {"Tunnel.TTL",                  config_parse_unsigned,              0,                             offsetof(Tunnel, ttl)},
      {"Tunnel.Mode",                 config_parse_ip6tnl_mode,           0,                             offsetof(Tunnel, ip6tnl_mode)},
      {(char*)0},
      {"Tap.User",                    config_parse_string,                0,                             offsetof(TunTap, user_name)},
      {"Tun.MultiQueue",              config_parse_bool,                  0,                             offsetof(TunTap, multi_queue)},
      {"NetDev.MTUBytes",             config_parse_iec_size,              0,                             offsetof(NetDev, mtu)},
      {"NetDev.Name",                 config_parse_ifname,                0,                             offsetof(NetDev, ifname)},
      {(char*)0},
      {"NetDev.Description",          config_parse_string,                0,                             offsetof(NetDev, description)},
      {"Tap.MultiQueue",              config_parse_bool,                  0,                             offsetof(TunTap, multi_queue)},
      {(char*)0},
      {"VXLAN.Group",                 config_parse_vxlan_group_address,   0,                             offsetof(VxLan, group)},
      {"Tunnel.DiscoverPathMTU",      config_parse_bool,                  0,                             offsetof(Tunnel, pmtudisc)},
      {"Bond.GratuitousARP",          config_parse_unsigned,              0,                             offsetof(Bond, num_grat_arp)},
      {(char*)0},
      {"VXLAN.PortRange",             config_parse_port_range,            0,                             0},
      {"Bridge.MaxAgeSec",            config_parse_sec,                   0,                             offsetof(Bridge, max_age)},
      {(char*)0},
      {"Bridge.MulticastQuerier",     config_parse_tristate,              0,                             offsetof(Bridge, mcast_querier)},
      {"Peer.Name",                   config_parse_ifname,                0,                             offsetof(Veth, ifname_peer)},
      {"Bond.ResendIGMP",             config_parse_unsigned,              0,                             offsetof(Bond, resend_igmp)},
      {"VXLAN.DestinationPort",       config_parse_destination_port,      0,                             offsetof(VxLan, dest_port)},
      {(char*)0},
      {"Bond.AdSelect",               config_parse_bond_ad_select,        0,                             offsetof(Bond, ad_select)},
      {"Tap.VNetHeader",              config_parse_bool,                  0,                             offsetof(TunTap, vnet_hdr)},
      {"Bridge.STP",                  config_parse_tristate,              0,                             offsetof(Bridge, stp)},
      {"VXLAN.GroupPolicyExtension",  config_parse_bool,                  0,                             offsetof(VxLan, group_policy)},
      {"Bond.LearnPacketIntervalSec", config_parse_sec,                   0,                             offsetof(Bond, lp_interval)},
      {"Bridge.DefaultPVID",          config_parse_vlanid,                0,                             offsetof(Bridge, default_pvid)},
      {"Tun.PacketInfo",              config_parse_bool,                  0,                             offsetof(TunTap, packet_info)},
      {"Tunnel.CopyDSCP",             config_parse_bool,                  0,                             offsetof(Tunnel, copy_dscp)},
      {"Bond.ARPValidate",            config_parse_bond_arp_validate,     0,                             offsetof(Bond, arp_validate)},
      {"VXLAN.UDPChecksum",           config_parse_bool,                  0,                             offsetof(VxLan, udpcsum)},
      {"Tunnel.Remote",               config_parse_tunnel_address,        0,                             offsetof(Tunnel, remote)},
      {"Tap.PacketInfo",              config_parse_bool,                  0,                             offsetof(TunTap, packet_info)},
      {"Bridge.Priority",             config_parse_uint16,                0,                             offsetof(Bridge, priority)},
      {(char*)0},
      {"MACVTAP.Mode",                config_parse_macvlan_mode,          0,                             offsetof(MacVlan, mode)},
      {"VXLAN.MaximumFDBEntries",     config_parse_unsigned,              0,                             offsetof(VxLan, max_fdb)},
      {"Bridge.HelloTimeSec",         config_parse_sec,                   0,                             offsetof(Bridge, hello_time)},
      {"Bond.UpDelaySec",             config_parse_sec,                   0,                             offsetof(Bond, updelay)},
      {(char*)0},
      {"NetDev.MACAddress",           config_parse_hwaddr,                0,                             offsetof(NetDev, mac)},
      {"VXLAN.FDBAgeingSec",          config_parse_sec,                   0,                             offsetof(VxLan, fdb_ageing)},
      {"Bridge.MulticastSnooping",    config_parse_tristate,              0,                             offsetof(Bridge, mcast_snooping)},
      {"Match.Host",                  config_parse_net_condition,         CONDITION_HOST,                offsetof(NetDev, match_host)},
      {"VRF.TableId",                 config_parse_uint32,                0,                             offsetof(Vrf, table_id)},
      {"Bridge.ForwardDelaySec",      config_parse_sec,                   0,                             offsetof(Bridge, forward_delay)},
      {"Bond.TransmitHashPolicy",     config_parse_bond_xmit_hash_policy, 0,                             offsetof(Bond, xmit_hash_policy)},
      {"VXLAN.UDP6ZeroChecksumTx",    config_parse_bool,                  0,                             offsetof(VxLan, udp6zerocsumtx)},
      {"Peer.MACAddress",             config_parse_hwaddr,                0,                             offsetof(Veth, mac_peer)},
      {"IPVLAN.Mode",                 config_parse_ipvlan_mode,           0,                             offsetof(IPVlan, mode)},
      {"VXLAN.UDPCheckSum",           config_parse_bool,                  0,                             offsetof(VxLan, udpcsum)},
      {"Bond.MIIMonitorSec",          config_parse_sec,                   0,                             offsetof(Bond, miimon)},
      {"VXLAN.L3MissNotification",    config_parse_bool,                  0,                             offsetof(VxLan, l3miss)},
      {"Bond.PacketsPerSlave",        config_parse_unsigned,              0,                             offsetof(Bond, packets_per_slave)},
      {"Bond.LACPTransmitRate",       config_parse_bond_lacp_rate,        0,                             offsetof(Bond, lacp_rate)},
      {"VXLAN.MacLearning",           config_parse_bool,                  0,                             offsetof(VxLan, learning)},
      {"VXLAN.RouteShortCircuit",     config_parse_bool,                  0,                             offsetof(VxLan, route_short_circuit)},
      {"VXLAN.ARPProxy",              config_parse_bool,                  0,                             offsetof(VxLan, arp_proxy)},
      {"Tunnel.InputKey",             config_parse_tunnel_key,            0,                             offsetof(Tunnel, ikey)},
      {(char*)0},
      {"VXLAN.RemoteChecksumTx",      config_parse_bool,                  0,                             offsetof(VxLan, remote_csum_tx)},
      {"Bond.ARPAllTargets",          config_parse_bond_arp_all_targets,  0,                             offsetof(Bond, arp_all_targets)},
      {"Bond.Mode",                   config_parse_bond_mode,             0,                             offsetof(Bond, mode)},
      {"Tunnel.TOS",                  config_parse_unsigned,              0,                             offsetof(Tunnel, tos)},
      {"Bond.PrimaryReselectPolicy",  config_parse_bond_primary_reselect, 0,                             offsetof(Bond, primary_reselect)},
      {"Bond.ARPIPTargets",           config_parse_arp_ip_target_address, 0,                             0},
      {(char*)0},
      {"VXLAN.UDP6ZeroCheckSumTx",    config_parse_bool,                  0,                             offsetof(VxLan, udp6zerocsumtx)},
      {"Match.Virtualization",        config_parse_net_condition,         CONDITION_VIRTUALIZATION,      offsetof(NetDev, match_virt)},
      {"Tunnel.OutputKey",            config_parse_tunnel_key,            0,                             offsetof(Tunnel, okey)},
      {"VLAN.Id",                     config_parse_vlanid,                0,                             offsetof(VLan, id)},
      {"VXLAN.Id",                    config_parse_uint64,                0,                             offsetof(VxLan, id)},
      {"Bond.ARPIntervalSec",         config_parse_sec,                   0,                             offsetof(Bond, arp_interval)},
      {"Tunnel.Key",                  config_parse_tunnel_key,            0,                             offsetof(Tunnel, key)},
      {"NetDev.Kind",                 config_parse_netdev_kind,           0,                             offsetof(NetDev, kind)},
      {"Tunnel.Local",                config_parse_tunnel_address,        0,                             offsetof(Tunnel, local)},
      {"Bond.MinLinks",               config_parse_unsigned,              0,                             offsetof(Bond, min_links)},
      {"VXLAN.UDP6ZeroChecksumRx",    config_parse_bool,                  0,                             offsetof(VxLan, udp6zerocsumrx)},
      {"Bond.AllSlavesActive",        config_parse_unsigned,              0,                             offsetof(Bond, all_slaves_active)},
      {(char*)0},
      {"Bond.DownDelaySec",           config_parse_sec,                   0,                             offsetof(Bond, downdelay)},
      {(char*)0},
      {"VXLAN.TTL",                   config_parse_unsigned,              0,                             offsetof(VxLan, ttl)},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"Match.Architecture",          config_parse_net_condition,         CONDITION_ARCHITECTURE,        offsetof(NetDev, match_arch)},
      {"VXLAN.L2MissNotification",    config_parse_bool,                  0,                             offsetof(VxLan, l2miss)},
      {"Bridge.AgeingTimeSec",        config_parse_sec,                   0,                             offsetof(Bridge, ageing_time)},
      {(char*)0},
      {"VXLAN.RemoteChecksumRx",      config_parse_bool,                  0,                             offsetof(VxLan, remote_csum_rx)},
      {(char*)0},
      {"VXLAN.TOS",                   config_parse_unsigned,              0,                             offsetof(VxLan, tos)},
      {(char*)0}, {(char*)0},
      {"Bond.FailOverMACPolicy",      config_parse_bond_fail_over_mac,    0,                             offsetof(Bond, fail_over_mac)},
      {(char*)0},
      {"VXLAN.UDP6ZeroCheckSumRx",    config_parse_bool,                  0,                             offsetof(VxLan, udp6zerocsumrx)},
      {(char*)0}, {(char*)0},
      {"Tun.OneQueue",                config_parse_bool,                  0,                             offsetof(TunTap, one_queue)},
      {(char*)0},
      {"Tun.Group",                   config_parse_string,                0,                             offsetof(TunTap, group_name)},
      {"Bridge.VLANFiltering",        config_parse_tristate,              0,                             offsetof(Bridge, vlan_filtering)},
      {(char*)0},
      {"Tap.OneQueue",                config_parse_bool,                  0,                             offsetof(TunTap, one_queue)},
      {(char*)0},
      {"Tap.Group",                   config_parse_string,                0,                             offsetof(TunTap, group_name)},
      {(char*)0}, {(char*)0},
      {"MACVLAN.Mode",                config_parse_macvlan_mode,          0,                             offsetof(MacVlan, mode)},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {"Tunnel.EncapsulationLimit",   config_parse_encap_limit,           0,                             offsetof(Tunnel, encap_limit)},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0},
      {"Tunnel.IPv6FlowLabel",        config_parse_ipv6_flowlabel,        0,                             offsetof(Tunnel, ipv6_flowlabel)},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {"Match.KernelCommandLine",     config_parse_net_condition,         CONDITION_KERNEL_COMMAND_LINE, offsetof(NetDev, match_kernel)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = network_netdev_gperf_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].section_and_lvalue;

          if (s && *str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
