#include "../libvxi11/vxi11_user.h"

#include "penguin_signalG.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one signal generator. 
 */
typedef struct penguin_signalG_s{
	char	device_ip[64];
	CLINK		*clink;
	char	message[256];
}penguin_signal_t;

static penguin_signal_t p;

/*
int penguin_signalG_init(){

	p.clink = new CLINK;

	memset(p.device_ip, 0, 64);
	memset(p.message, 0, 256);

	sprintf(p.device_ip, "192.168.11.%d", 4);

	int ret=vxi11_open_device(p.device_ip, p.clink);

	if (ret != 0) {
		printf("Error: could not open device %s, quitting\n", p.device_ip);
		exit(2);
		}

}
*/

int penguin_signalG_init() {
    // Initialize clink object
    p.clink = new CLINK;

    // Ensure memory is properly allocated
    if (p.clink == NULL) {
        printf("[ERROR] Failed to allocate memory for clink.\n");
        exit(1);
    }
    //printf("[DEBUG] clink object allocated at address: %p\n", (void *)p.clink);

    // Clear memory for device_ip and message
    memset(p.device_ip, 0, sizeof(p.device_ip));
    memset(p.message, 0, sizeof(p.message));

    // Set the device IP address
    sprintf(p.device_ip, "192.168.11.%d", 4);

    // Attempt to open the device
    int ret = vxi11_open_device(p.device_ip, p.clink);

    // Check the return value and log detailed debug information
    if (ret != 0) {
        printf("[ERROR] vxi11_open_device failed for IP %s.\n", p.device_ip);
        printf("[ERROR] Return code: %d\n", ret);
        printf("[DEBUG] Possible causes:\n");
        printf("        - Incorrect IP address\n");
        printf("        - Device not configured for VXI-11\n");
        printf("        - Network issues or blocked ports\n");
        printf("        - VXI-11 service not running on the device\n");

        if (p.clink) {
       //     printf("[DEBUG] clink details: client=%p, link=%p\n", (void *)p.clink->client, (void *)p.clink->link);
        } else {
            printf("[DEBUG] clink object is NULL.\n");
        }

        delete p.clink; // Cleanup allocated memory before exiting
        exit(2);
    }

    // Additional debug information for successful connection
    if (p.clink) {
    //    printf("[DEBUG] clink successfully initialized:\n");
    //    printf("        - client=%p\n", (void *)p.clink->client);
    //    printf("        - link=%p\n", (void *)p.clink->link);
    } else {
        printf("[WARNING] clink is NULL despite successful connection.\n");
    }

    // If successful
    printf("penguin_signalG_Init(); Successfully connected to device at IP: %s\n", p.device_ip);
    return 0;
}


int penguin_signalG_end(){
	int ret=vxi11_close_device(p.device_ip, p.clink);
    if (ret != 0) {
        printf("Error: failed to close device %s, return code: %d\n", p.device_ip, ret);
        return ret; // エラーコードを呼び出し元に返す
    }
    delete p.clink; // メモリを解放
    p.clink = NULL;
	return 0;
}

void penguin_signalG_setFrequency(double value){
	char tmp[256];
	//printf("penguin_signalG_setFrequency(%f) implemented?\n", value);
	sprintf(tmp, "FREQ %.10e GHz", value);
	printf("penguin_signalG_setFrequency() command: %s\n", tmp);
    if (p.clink == NULL) {
        printf("[ERROR] penguin_signalG_setFrequency: clink is NULL\n");
        return;
    }
        penguin_signalG_send_command(tmp);

//	printf("penguin_signalG_setFrequency(%f) not implemented yet.\n", value);
}

void penguin_signalG_setPower(double value){
	char tmp[256];
//	printf("penguin_signalG_setPower(%f) implemented?\n", value);
	sprintf(tmp, "POW:AMPL %.2e dBm", value);
	penguin_signalG_send_command(tmp);

//	printf("penguin_signalG_setPower(%f) not implemented yet.\n", value);
}

double penguin_signalG_getFrequency(){
//        printf("penguin_signalG_getFrequency()");
	long int ret = penguin_signalG_send_and_receive("FREQ:CW?");
	if(ret == 0)
		return 0.0;
	return strtod(p.message, NULL);
}
double penguin_signalG_getPower(){
//        printf("penguin_signalG_getPower()");
	int ret = penguin_signalG_send_and_receive("POW:AMPL?");
	if(ret == 0)
		return 0.0;
	return strtod(p.message, NULL);
}

void penguin_signalG_setOutput(int isOn){
	char tmp[256];
	printf("penguin_signalG_setOutput(%d) implemented?\n", isOn);
	if(isOn){
		sprintf(tmp, "OUTP:STAT ON");
	}else{
		sprintf(tmp, "OUTP:STAT OFF");
	}
	penguin_signalG_send_command(tmp);

//	printf("penguin_signalG_setPower(%f) not implemented yet.\n", value);
}

/* medium level API */
int penguin_signalG_send_and_receive_int(const char* cmd){
}


/* low level API */
long penguin_signalG_send_and_receive(const char* cmd){
    if (cmd == NULL || strlen(cmd) == 0) {
        printf("[ERROR] penguin_signalG_send_and_receive: cmd is NULL or empty.\n");
        return -1;
    }
	if (strstr(cmd, "?") == 0) {
		printf("penguin_singalG_send_and_receive(), command(%s) is not query!\n");
		return -1;
	}

	penguin_signalG_send_command(cmd);

	long bytes_returned = vxi11_receive(p.clink, p.message, 256);
	if (bytes_returned == -15) {
		//printf("*** [ NOTHING RECEIVED ] ***\n");
		return 0;
	}

	if (bytes_returned > 0) {
		//printf("%s\n", p.message);
	}
	return bytes_returned;
}

long penguin_signalG_receive() {
 //   printf("[DEBUG] Starting to receive data...\n");

    if (p.clink == NULL) {
        printf("[ERROR] clink object is NULL. Cannot receive data.\n");
        return -1; // Early return if clink is not properly initialized
    }

   // printf("[DEBUG] clink details: client=%p, link=%p\n", (void *)p.clink->client, (void *)p.clink->link);

    // Receive data
    long bytes_returned = vxi11_receive(p.clink, p.message, 256);
  //  printf("[DEBUG] vxi11_receive returned: %ld\n", bytes_returned);

    if (bytes_returned == -15) {
        printf("[WARNING] No data received. Possible causes:\n");
        printf("          - Timeout occurred.\n");
        printf("          - Device did not respond.\n");
        return 0;
    }

    if (bytes_returned > 0) {
        printf("[INFO] Successfully received response (%ld bytes): %s\n", bytes_returned, p.message);
    } else {
        printf("[ERROR] Failed to receive data. Return code: %ld\n", bytes_returned);
        printf("[DEBUG] Possible issues:\n");
        printf("        - Network problem or disconnection.\n");
        printf("        - Device may not be configured properly.\n");
        printf("        - Device may not support the requested command.\n");
    }

    return bytes_returned;
}

int penguin_signalG_send_command(const char* cmd) {
    // デバッグ: 引数の確認
    if (cmd == NULL) {
        printf("penguin_signalG_send_command: Error: cmd is NULL\n");
        return -1;
    }
    printf("penguin_signalG_send_command: Received command: %s\n", cmd);

    // デバッグ: clink 構造体のチェック
    if (p.clink == NULL) {
        printf("penguin_signalG_send_command: Error: clink is NULL\n");
        return -1;
    }

    //printf("penguin_signalG_send_command: Sending command to device...\n");

    // vxi11_send 呼び出し
    int ret = vxi11_send(p.clink, cmd);
    if (ret < 0) {
        printf("penguin_signalG_send_command: vxi11_send failed with error code: %d, cmd: %s\n", ret, cmd);
        return -1;
    }

    // デバッグ: 成功時
    //printf("penguin_signalG_send_command: Command sent successfully: %s\n", cmd);

    return 0;
}

//int penguin_signalG_send_command(const char* cmd){
//	if (vxi11_send(p.clink, cmd) < 0){
//		printf("penguin_singalG_send_command(%s) error.\n", cmd);
//		return -1;
//	}
//	return 0;
//}

char* penguin_signalG_getMassage(){
	return p.message;
}


