#include <libqt6c.h>
#include <stdio.h>

void onFinished(void* dns) {
    q_dnslookup_delete_later(dns);

    if (q_dnslookup_error(dns) != QDNSLOOKUP_ERROR_NOERROR) {
        const char* error_str = q_dnslookup_error_string(dns);
        printf("DNS lookup failed: %s\n", error_str);
        libqt_free(error_str);
        return;
    }

    libqt_list records = q_dnslookup_host_address_records(dns);
    printf("Found %zu results.\n", records.len);

    QDnsHostAddressRecord** results = (QDnsHostAddressRecord**)records.data.ptr;
    for (size_t i = 0; i < records.len; i++) {
        QHostAddress* value = q_dnshostaddressrecord_value(results[i]);
        const char* addr_str = q_hostaddress_to_string(value);
        printf("- %s\n", addr_str);
        libqt_free(addr_str);
        q_hostaddress_delete(value);
    }
    q_dnshostaddressrecord_delete(records.data.ptr);

    q_application_exit();
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    printf("Looking up DNS info, please wait...");

    QDnsLookup* dns = q_dnslookup_new2(QDNSLOOKUP_TYPE_A, "google.com");
    q_dnslookup_on_finished(dns, onFinished);
    q_dnslookup_lookup(dns);

    return q_application_exec();
}
