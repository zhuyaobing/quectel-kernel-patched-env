/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf  */
/* Computed positions: -k'7,11-12,$' */

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
#include "networkd.h"
#include "networkd-conf.h"
#include "network-internal.h"
#include "vlan-util.h"
#include <string.h>

#define TOTAL_KEYWORDS 116
#define MIN_WORD_LENGTH 8
#define MAX_WORD_LENGTH 69
#define MIN_HASH_VALUE 11
#define MAX_HASH_VALUE 200
/* maximum key range = 190, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
network_network_gperf_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201,   0,  45, 201, 201,
      201, 201,  70, 201,  15, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201,  60,  65,  75,  40,  80,
       65,  45,  15,  45, 201,  45,  45,  55,  35, 201,
       15, 201,  10,   0,  55,  35,  55, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201,   0,   0,  45,
       10,   5,  95,   5,   5,  40, 201,  15,  40,   5,
       15,   0,   5, 201,   0,   0,  30,  45,  10,  60,
       85,  15, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
      201, 201, 201, 201, 201, 201
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
      case 9:
      case 8:
      case 7:
        hval += asso_values[(unsigned char)str[6]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct ConfigPerfItem *
network_network_gperf_lookup (register const char *str, register unsigned int len)
{
  static const struct ConfigPerfItem wordlist[] =
    {
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"DHCP.UseDNS",                            config_parse_bool,                              0,                             offsetof(Network, dhcp_use_dns)},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0},
      {"Address.Peer",                           config_parse_address,                           0,                             0},
      {(char*)0}, {(char*)0},
      {"DHCP.UseDomains",                        config_parse_dhcp_use_domains,                  0,                             offsetof(Network, dhcp_use_domains)},
      {"Route.Scope",                            config_parse_route_scope,                       0,                             0},
      {(char*)0},
      {"DHCP.Hostname",                          config_parse_hostname,                          0,                             offsetof(Network, dhcp_hostname)},
      {(char*)0},
      {"Address.Address",                        config_parse_address,                           0,                             0},
      {"Network.DNS",                            config_parse_dns,                               0,                             0},
      {"DHCP.SendHostname",                      config_parse_bool,                              0,                             offsetof(Network, dhcp_send_hostname)},
      {"DHCP.UseDomainName",                     config_parse_dhcp_use_domains,                  0,                             offsetof(Network, dhcp_use_domains)},
      {"Address.HomeAddress",                    config_parse_address_flags,                     0,                             0},
      {"Match.Path",                             config_parse_strv,                              0,                             offsetof(Network, match_path)},
      {"DHCP.UseTimezone",                       config_parse_bool,                              0,                             offsetof(Network, dhcp_use_timezone)},
      {(char*)0},
      {"Link.ARP",                               config_parse_tristate,                          0,                             offsetof(Network, arp)},
      {(char*)0},
      {"Network.Domains",                        config_parse_domains,                           0,                             0},
      {(char*)0}, {(char*)0},
      {"DHCPv4.UseHostname",                     config_parse_bool,                              0,                             offsetof(Network, dhcp_use_hostname)},
      {"DHCPServer.PoolSize",                    config_parse_uint32,                            0,                             offsetof(Network, dhcp_server_pool_size)},
      {"Network.Address",                        config_parse_address,                           0,                             0},
      {"DHCP.UseNTP",                            config_parse_bool,                              0,                             offsetof(Network, dhcp_use_ntp)},
      {(char*)0}, {(char*)0},
      {"Bridge.HairPin",                         config_parse_bool,                              0,                             offsetof(Network, hairpin)},
      {"Address.ManageTemporaryAddress",         config_parse_address_flags,                     0,                             0},
      {"Route.PreferredSource",                  config_parse_preferred_src,                     0,                             0},
      {"Address.Broadcast",                      config_parse_broadcast,                         0,                             0},
      {(char*)0},
      {"Network.DNSSECNegativeTrustAnchors",     config_parse_dnssec_negative_trust_anchors,     0,                             0},
      {"Match.Name",                             config_parse_ifnames,                           0,                             offsetof(Network, match_name)},
      {"DHCP.UseHostname",                       config_parse_bool,                              0,                             offsetof(Network, dhcp_use_hostname)},
      {(char*)0}, {(char*)0},
      {"DHCPServer.DNS",                         config_parse_dhcp_server_dns,                   0,                             0},
      {"Match.Host",                             config_parse_net_condition,                     CONDITION_HOST,                offsetof(Network, match_host)},
      {"Network.NTP",                            config_parse_strv,                              0,                             offsetof(Network, ntp)},
      {"Match.Driver",                           config_parse_strv,                              0,                             offsetof(Network, match_driver)},
      {"Address.Label",                          config_parse_label,                             0,                             0},
      {"Network.BindCarrier",                    config_parse_strv,                              0,                             offsetof(Network, bind_carrier)},
      {(char*)0},
      {"Address.AutoJoin",                       config_parse_address_flags,                     0,                             0},
      {"Network.Bond",                           config_parse_netdev,                            0,                             offsetof(Network, bond)},
      {(char*)0},
      {"DHCPServer.NTP",                         config_parse_dhcp_server_ntp,                   0,                             0},
      {"DHCPv4.UseDomainName",                   config_parse_dhcp_use_domains,                  0,                             offsetof(Network, dhcp_use_domains)},
      {"DHCPServer.PoolOffset",                  config_parse_uint32,                            0,                             offsetof(Network, dhcp_server_pool_offset)},
      {"Route.Source",                           config_parse_destination,                       0,                             0},
      {(char*)0},
      {"Network.IPv6PrivacyExtensions",          config_parse_ipv6_privacy_extensions,           0,                             offsetof(Network, ipv6_privacy_extensions)},
      {"Match.Type",                             config_parse_strv,                              0,                             offsetof(Network, match_type)},
      {"Bridge.Cost",                            config_parse_unsigned,                          0,                             offsetof(Network, cost)},
      {"Network.IPv6Token",                      config_parse_ipv6token,                         0,                             offsetof(Network, ipv6_token)},
      {"DHCP.DUIDType",                          config_parse_duid_type,                         0,                             offsetof(Network, duid.type)},
      {"Bridge.UnicastFlood",                    config_parse_bool,                              0,                             offsetof(Network, unicast_flood)},
      {"DHCP.RouteTable",                        config_parse_dhcp_route_table,                  0,                             offsetof(Network, dhcp_route_table)},
      {"Route.Table",                            config_parse_route_table,                       0,                             0},
      {"Network.LinkLocalAddressing",            config_parse_address_family_boolean,            0,                             offsetof(Network, link_local)},
      {"Network.IPv6AcceptRouterAdvertisements", config_parse_tristate,                          0,                             offsetof(Network, ipv6_accept_ra)},
      {"DHCPServer.Timezone",                    config_parse_timezone,                          0,                             offsetof(Network, dhcp_server_timezone)},
      {"Network.Gateway",                        config_parse_gateway,                           0,                             0},
      {"DHCP.UseMTU",                            config_parse_bool,                              0,                             offsetof(Network, dhcp_use_mtu)},
      {(char*)0},
      {"DHCP.CriticalConnection",                config_parse_bool,                              0,                             offsetof(Network, dhcp_critical)},
      {"Network.Bridge",                         config_parse_netdev,                            0,                             offsetof(Network, bridge)},
      {"Link.MACAddress",                        config_parse_hwaddr,                            0,                             offsetof(Network, mac)},
      {"DHCP.RequestBroadcast",                  config_parse_bool,                              0,                             offsetof(Network, dhcp_broadcast)},
      {(char*)0},
      {"DHCPv4.UseDNS",                          config_parse_bool,                              0,                             offsetof(Network, dhcp_use_dns)},
      {"DHCP.UseRoutes",                         config_parse_bool,                              0,                             offsetof(Network, dhcp_use_routes)},
      {"Network.IPv6HopLimit",                   config_parse_int,                               0,                             offsetof(Network, ipv6_hop_limit)},
      {"Match.MACAddress",                       config_parse_hwaddr,                            0,                             offsetof(Network, match_mac)},
      {"Network.IPv6DuplicateAddressDetection",  config_parse_int,                               0,                             offsetof(Network, ipv6_dad_transmits)},
      {"Address.DuplicateAddressDetection",      config_parse_address_flags,                     0,                             0},
      {"Network.Description",                    config_parse_string,                            0,                             offsetof(Network, description)},
      {"Network.IPMasquerade",                   config_parse_bool,                              0,                             offsetof(Network, ip_masquerade)},
      {"Bridge.FastLeave",                       config_parse_bool,                              0,                             offsetof(Network, fast_leave)},
      {"Network.LLDP",                           config_parse_lldp_mode,                         0,                             offsetof(Network, lldp_mode)},
      {"DHCPServer.EmitDNS",                     config_parse_bool,                              0,                             offsetof(Network, dhcp_server_emit_dns)},
      {"Network.Tunnel",                         config_parse_tunnel,                            0,                             0},
      {(char*)0},
      {"DHCPServer.EmitRouter",                  config_parse_bool,                              0,                             offsetof(Network, dhcp_server_emit_router)},
      {(char*)0},
      {"Link.MTUBytes",                          config_parse_iec_size,                          0,                             offsetof(Network, mtu)},
      {"Network.DNSSEC",                         config_parse_dnssec_mode,                       0,                             offsetof(Network, dnssec_mode)},
      {"Network.MulticastDNS",                   config_parse_resolve_support,                   0,                             offsetof(Network, mdns)},
      {"DHCP.VendorClassIdentifier",             config_parse_string,                            0,                             offsetof(Network, dhcp_vendor_class_identifier)},
      {"Network.IPForward",                      config_parse_address_family_boolean_with_kernel,0,                             offsetof(Network, ip_forward)},
      {"DHCPServer.EmitTimezone",                config_parse_bool,                              0,                             offsetof(Network, dhcp_server_emit_timezone)},
      {"DHCP.IAID",                              config_parse_iaid,                              0,                             offsetof(Network, iaid)},
      {"DHCPv4.CriticalConnection",              config_parse_bool,                              0,                             offsetof(Network, dhcp_critical)},
      {"DHCP.DUIDRawData",                       config_parse_duid_rawdata,                      0,                             offsetof(Network, duid)},
      {(char*)0},
      {"DHCPServer.EmitNTP",                     config_parse_bool,                              0,                             offsetof(Network, dhcp_server_emit_ntp)},
      {"Bridge.AllowPortToBeRoot",               config_parse_bool,                              0,                             offsetof(Network, allow_port_to_be_root)},
      {"DHCPServer.DefaultLeaseTimeSec",         config_parse_sec,                               0,                             offsetof(Network, dhcp_server_default_lease_time_usec)},
      {"Network.EmitLLDP",                       config_parse_lldp_emit,                         0,                             offsetof(Network, lldp_emit)},
      {"/* legacy alias for the above */"},
      {"Match.KernelCommandLine",                config_parse_net_condition,                     CONDITION_KERNEL_COMMAND_LINE, offsetof(Network, match_kernel)},
      {"Network.IPv4LLRoute",                    config_parse_bool,                              0,                             offsetof(Network, ipv4ll_route)},
      {"Network.IPv6AcceptRA",                   config_parse_tristate,                          0,                             offsetof(Network, ipv6_accept_ra)},
      {"DHCP.RouteMetric",                       config_parse_unsigned,                          0,                             offsetof(Network, dhcp_route_metric)},
      {(char*)0},
      {"Network.DHCPServer",                     config_parse_bool,                              0,                             offsetof(Network, dhcp_server)},
      {"Address.PrefixRoute",                    config_parse_address_flags,                     0,                             0},
      {"BridgeVLAN.PVID",                        config_parse_brvlan_pvid,                       0,                             0},
      {"DHCPServer.MaxLeaseTimeSec",             config_parse_sec,                               0,                             offsetof(Network, dhcp_server_max_lease_time_usec)},
      {"Route.Destination",                      config_parse_destination,                       0,                             0},
      {"Network.LLMNR",                          config_parse_resolve_support,                   0,                             offsetof(Network, llmnr)},
      {"Bridge.UseBPDU",                         config_parse_bool,                              0,                             offsetof(Network, use_bpdu)},
      {"Address.PreferredLifetime",              config_parse_lifetime,                          0,                             0},
      {"Network.ProxyARP",                       config_parse_tristate,                          0,                             offsetof(Network, proxy_arp)},
      {"Network.DHCP",                           config_parse_dhcp,                              0,                             offsetof(Network, dhcp)},
      {"Route.Gateway",                          config_parse_gateway,                           0,                             0},
      {"IPv6AcceptRA.UseDNS",                    config_parse_bool,                              0,                             offsetof(Network, ipv6_accept_ra_use_dns)},
      {"Match.Virtualization",                   config_parse_net_condition,                     CONDITION_VIRTUALIZATION,      offsetof(Network, match_virt)},
      {"DHCP.ClientIdentifier",                  config_parse_dhcp_client_identifier,            0,                             offsetof(Network, dhcp_client_identifier)},
      {(char*)0},
      {"IPv6AcceptRA.UseDomains",                config_parse_dhcp_use_domains,                  0,                             offsetof(Network, ipv6_accept_ra_use_domains)},
      {"/* backwards compatibility: do not add new entries to this section */"},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"IPv6AcceptRA.RouteTable",                config_parse_dhcp_route_table,                  0,                             offsetof(Network, ipv6_accept_ra_route_table)},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0},
      {"Match.Architecture",                     config_parse_net_condition,                     CONDITION_ARCHITECTURE,        offsetof(Network, match_arch)},
      {"Network.IPv4LL",                         config_parse_ipv4ll,                            0,                             offsetof(Network, link_local)},
      {(char*)0},
      {"Network.VRF",                            config_parse_netdev,                            0,                             0},
      {"Network.VLAN",                           config_parse_netdev,                            0,                             0},
      {"DHCPv4.UseMTU",                          config_parse_bool,                              0,                             offsetof(Network, dhcp_use_mtu)},
      {(char*)0},
      {"BridgeVLAN.VLAN",                        config_parse_brvlan_vlan,                       0,                             0},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"Network.IPVLAN",                         config_parse_netdev,                            0,                             0},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"Network.VXLAN",                          config_parse_netdev,                            0,                             0},
      {(char*)0},
      {"BridgeVLAN.EgressUntagged",              config_parse_brvlan_untagged,                   0,                             0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {"Network.MACVTAP",                        config_parse_netdev,                            0,                             0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"BridgeFDB.VLANId",                       config_parse_fdb_vlan_id,                       0,                             0},
      {(char*)0}, {(char*)0}, {(char*)0},
      {"Network.MACVLAN",                        config_parse_netdev,                            0,                             0},
      {(char*)0},
      {"Route.Metric",                           config_parse_route_priority,                    0,                             0},
      {(char*)0}, {(char*)0},
      {"BridgeFDB.MACAddress",                   config_parse_fdb_hwaddr,                        0,                             0}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = network_network_gperf_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].section_and_lvalue;

          if (s && *str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
