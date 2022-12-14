const char *dns_type_to_string(int type) {
	switch(type) {
        case DNS_TYPE_A: return "A";
        case DNS_TYPE_NS: return "NS";
        case DNS_TYPE_MD: return "MD";
        case DNS_TYPE_MF: return "MF";
        case DNS_TYPE_CNAME: return "CNAME";
        case DNS_TYPE_SOA: return "SOA";
        case DNS_TYPE_MB: return "MB";
        case DNS_TYPE_MG: return "MG";
        case DNS_TYPE_MR: return "MR";
        case DNS_TYPE_NULL: return "NULL";
        case DNS_TYPE_WKS: return "WKS";
        case DNS_TYPE_PTR: return "PTR";
        case DNS_TYPE_HINFO: return "HINFO";
        case DNS_TYPE_MINFO: return "MINFO";
        case DNS_TYPE_MX: return "MX";
        case DNS_TYPE_TXT: return "TXT";
        case DNS_TYPE_RP: return "RP";
        case DNS_TYPE_AFSDB: return "AFSDB";
        case DNS_TYPE_X25: return "X25";
        case DNS_TYPE_ISDN: return "ISDN";
        case DNS_TYPE_RT: return "RT";
        case DNS_TYPE_NSAP: return "NSAP";
        case DNS_TYPE_NSAP_PTR: return "NSAP-PTR";
        case DNS_TYPE_SIG: return "SIG";
        case DNS_TYPE_KEY: return "KEY";
        case DNS_TYPE_PX: return "PX";
        case DNS_TYPE_GPOS: return "GPOS";
        case DNS_TYPE_AAAA: return "AAAA";
        case DNS_TYPE_LOC: return "LOC";
        case DNS_TYPE_NXT: return "NXT";
        case DNS_TYPE_EID: return "EID";
        case DNS_TYPE_NIMLOC: return "NIMLOC";
        case DNS_TYPE_SRV: return "SRV";
        case DNS_TYPE_ATMA: return "ATMA";
        case DNS_TYPE_NAPTR: return "NAPTR";
        case DNS_TYPE_KX: return "KX";
        case DNS_TYPE_CERT: return "CERT";
        case DNS_TYPE_A6: return "A6";
        case DNS_TYPE_DNAME: return "DNAME";
        case DNS_TYPE_SINK: return "SINK";
        case DNS_TYPE_OPT: return "OPT";
        case DNS_TYPE_APL: return "APL";
        case DNS_TYPE_DS: return "DS";
        case DNS_TYPE_SSHFP: return "SSHFP";
        case DNS_TYPE_IPSECKEY: return "IPSECKEY";
        case DNS_TYPE_RRSIG: return "RRSIG";
        case DNS_TYPE_NSEC: return "NSEC";
        case DNS_TYPE_DNSKEY: return "DNSKEY";
        case DNS_TYPE_DHCID: return "DHCID";
        case DNS_TYPE_NSEC3: return "NSEC3";
        case DNS_TYPE_NSEC3PARAM: return "NSEC3PARAM";
        case DNS_TYPE_TLSA: return "TLSA";
        case DNS_TYPE_HIP: return "HIP";
        case DNS_TYPE_NINFO: return "NINFO";
        case DNS_TYPE_RKEY: return "RKEY";
        case DNS_TYPE_TALINK: return "TALINK";
        case DNS_TYPE_CDS: return "CDS";
        case DNS_TYPE_CDNSKEY: return "CDNSKEY";
        case DNS_TYPE_OPENPGPKEY: return "OPENPGPKEY";
        case DNS_TYPE_SPF: return "SPF";
        case DNS_TYPE_NID: return "NID";
        case DNS_TYPE_L32: return "L32";
        case DNS_TYPE_L64: return "L64";
        case DNS_TYPE_LP: return "LP";
        case DNS_TYPE_EUI48: return "EUI48";
        case DNS_TYPE_EUI64: return "EUI64";
        case DNS_TYPE_TKEY: return "TKEY";
        case DNS_TYPE_TSIG: return "TSIG";
        case DNS_TYPE_IXFR: return "IXFR";
        case DNS_TYPE_AXFR: return "AXFR";
        case DNS_TYPE_MAILB: return "MAILB";
        case DNS_TYPE_MAILA: return "MAILA";
        case DNS_TYPE_ANY: return "ANY";
        case DNS_TYPE_URI: return "URI";
        case DNS_TYPE_CAA: return "CAA";
        case DNS_TYPE_TA: return "TA";
        case DNS_TYPE_DLV: return "DLV";
        default: return NULL;
	}
}

