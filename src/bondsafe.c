#include "bondsafe.h"


bool bt_scan_query(int dev_id, char* bond_mac_addr, int time_sec_ish) {
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int sock, len, flags;
    int i = 0;
    bool matched = false;
    char addr[19] = { 0 };
    char name[248] = { 0 };
   sock = hci_open_dev( dev_id );
   if (dev_id < 0 || sock < 0) {
        perror("Error opening socket, Quitting (Not doing anything).\n");
        exit(1);
    }

    len  = time_sec_ish;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("[Detected] %s  %s\n", addr, name);
	if(strncmp(bond_mac_addr, addr, MAC_ADDR_LEN) == 0) {
		matched = true;
		printf("Matches MAC!\h");
		}    
	}

    free( ii );
    close( sock );
    return matched;
}


int get_device() {
	return hci_get_route(NULL);
}

bool validate_args(int argc, char** argv) {
	int i = 0;
	if(true != isMAC(argv[0])) {
		fprintf(stderr,"Bad MAC Address\n");
		return false;
	}
	for(i = 1;i < argc;i++) {
		if(true != valid_arg_command(argv[i])) {
			fprintf(stderr,"Invalid argument: %s", argv[i]);
			return false;
		}
			
	}
	return true;
}
	
void scan_loop(char* bond_mac_addr, int limit_scans) { 
    int dev_id = get_device();
    int stop_count = 0;
    while(stop_count < limit_scans) {
	if(true == bt_scan_query(dev_id, bond_mac_addr, 15))
		stop_count = 0;
	else{
		stop_count++;
		printf("Device unaccounted, Strike %d/%d\n", stop_count, limit_scans);
	}
    }
	
}

void perform_actions(int action_count, char** action_strings) {
	printf("Horrid stuff really\n");
}

int main(int argc, char** argv) {
	char* bond_mac_addr = NULL;
	char** action_strns = NULL;
	int action_count = 0;
	if (argc < 2) {
		printf("bondsafe [MAC Address] <del/umt/cmd:PATH>\n");
		printf("\tMAC Address in 17 character format: 00:11:22:33:44:55\n");
		printf("\tRepeat as many rm/unmounts as you wish\n");
		return BAD_ARGS_ERROR;
	}
	argv++;
	argc--;
	if (true != validate_args(argc,argv)) {
		return BAD_ARGS_ERROR;
	}
	bond_mac_addr = argv[0];
	action_strns = argv+1;
	action_count = argc-1;
	
	scan_loop(bond_mac_addr, 4);
	perform_actions(action_count, action_strns);
	return OK_ERROR;
}
