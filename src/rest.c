#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <time.h>
#include <stdlib.h>

#include "cJSON.h"
#include "utils.h"

#define HOST "login.microsoftonline.com"
#define PORT "443"

/*
 * Function: poll_microsoft_for_token
 * ----------------------------------
 * *code: char array containing the device code used in the user's prompt.
 *
 * *resource_id: char array containing the resource id
 *
 * *client_id: char array containing the client id 
 *
 * *response_buf: empty buffer to include the response in.
 *
 * returns 0 if completion is successful, 1 if it fails.
 */

int poll_microsoft_for_token(char *code, const char *resource_id, const char *client_id, char *response_buf){
     /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[1024];

    /* Variables used to read the response from the server */
    int size;
    char buf[1024];

    char write_buf[2048];

    strcpy(response_buf, " ");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, HOST ":" PORT);

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
        printf("Connected\n");
    }

    strcpy(post_buf, "resource=");
    strcat(post_buf, resource_id);
    strcat(post_buf, "&code=");
    strcat(post_buf, code);
    strcat(post_buf, "&client_id=");
    strcat(post_buf, client_id);
    strcat(post_buf, "&grant_type=device_code");

    /* Data to create a HTTP request */
    strcpy(write_buf, "POST /");
    strcat(write_buf, "common/oauth2/token/ HTTP/1.1\r\n");
    strcat(write_buf, "Host: " HOST "\r\n");
    strcat(write_buf, "Connection: close \r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0 \r\n");
    strcat(write_buf, "Content-Length: 307\r\n");
    strcat(write_buf, "\r\n");
    strcat(write_buf, post_buf);
    strcat(write_buf, "\r\n");

    /* Attempts to write len bytes from buf to BIO */ 
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}

/*
 * Function: request_azure_oauth_token
 * -----------------------------------
 * *code: char array containing the device code used in the user's prompt.
 *
 * *resource_id: char array containing the resource id
 *
 * *client_id: char array containing the client id 
 *
 * returns a 0 if the function completes successfully, and 0 otherwise.
 */
int request_azure_oauth_token(char *code, const char *resource_id, const char *client_id, const char *token_buf){
    int start, end;
    char response_buf[9000];
    char json_buf[9000];
    cJSON *json; 
    char *access_token;
    poll_microsoft_for_token(code, resource_id, client_id, response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    printf("The json buffer is...%s\n", json_buf);
    json = cJSON_Parse(json_buf);
    cJSON *access = cJSON_GetObjectItem(json, "access_token");
    if (access == NULL){
        /* Something failed. */
        printf("Something failed.\n");
        strcpy(token_buf, "FAILURE");
        return 1;
   }
   printf("Has an access token!\n");
   strcpy(token_buf, cJSON_GetObjectItem(json, "access_token")->valuestring);
    return 0;
}

/*
 * Function: fill_json_buffer
 * --------------------------
 * *json_buf: char array that will hold the json message contained in raw_response
 * 
 * *raw_response: char array that holds the original raw http response from microsoft. 
 * 
 * *start holds the index of the '{' 
 *
 * *end holds the index of '}'
 *
*/

int fill_json_buffer(char *json_buf, char *raw_response, int *start, int *end){
    memcpy(json_buf, &raw_response[*start], *end - *start + 1);
    json_buf[*end + 1] = '\0'; 
    return 0;
}

/*
 * Function: get_client_id
 *------------------------
 * client_id: an empty buffer that will be filled with the client ID.
 *
 * returns a 0, which indicates the function completed correctly. This function is simple
 * so it is assumed there is no alternative return. 
 *
*/

int get_client_id(char *client_id){
    srand(time(NULL));
    int random = rand();
    sprintf(client_id, "%ld", random);
    return 0;
}
/*
 *
 * Function: find_json_bounds
 *---------------------------
 * json_buf: contains a full http response that includes json.
 * 
 * start:    pointer that will contain the index of the start of the json body. 
 *
 * end:      pointer that will contain the index of the end of the json body.
 *
 * returns an integer that indicates that the function successfully completed. 
 */ 

int find_json_bounds(char *json_buf, int *start, int *end){
    int i, j;
    for(i = 0; json_buf[i] != '{'; i++){}
    *start = i;
    for(j = i; json_buf[j] != '}'; j++){}
    *end = j;
    return 0;
}

int read_code_from_microsoft(const char *resource_id, const char *client_id, const char *tenant, char *response_buf){
    /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[2048];

    /* Variables used to read the response from the server */
    int size;
    char buf[2048];

    char write_buf[2048];

    strcpy(response_buf, " ");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, HOST ":" PORT);

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
        printf("Connected\n");
    }
    strcpy(post_buf, "resource=");
    strcat(post_buf, resource_id);
    strcat(post_buf, "&client_id=");
    strcat(post_buf, client_id);
    strcat(post_buf, "&client_request_id=");
    /* TODO: Change the below for a legitimate client request id, randomly generated */
    strcat(post_buf, "5929459294929");

    /* Data to create a HTTP request */
    strcpy(write_buf, "POST /");
    strcat(write_buf, tenant);
    strcat(write_buf, "/oauth2/devicecode/ HTTP/1.1\r\n");
    strcat(write_buf, "Host: " HOST "\r\n");
    strcat(write_buf, "Connection: close \r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0 \r\n");
    strcat(write_buf, "Content-Length: 100\r\n");
    strcat(write_buf, "\r\n");
    strcat(write_buf, post_buf);
    strcat(write_buf, "\r\n");
    printf("going to send %s\n", write_buf);

    /* Attempts to write len bytes from buf to BIO */ 
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}


int get_microsoft_graph_groups(char *token, char *response_buf){
    /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[2048];

    /* Variables used to read the response from the server */
    int size;
    char buf[2048];

    char write_buf[204800];

    strcpy(response_buf, "");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, "graph.microsoft.com:443");

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
        printf("Connected\n");
    }

    /* Data to create a HTTP request */
    strcat(write_buf, "GET /v1.0/me/memberOf/$/microsoft.graph.group HTTP/1.1\r\n");
    strcat(write_buf, "Authorization: Bearer ");
    strcat(write_buf, token);
    strcat(write_buf, "\r\n");
    strcat(write_buf, "Host: graph.microsoft.com\r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0\r\n");
    strcat(write_buf, "Connection: close\r\n");
    strcat(write_buf, "\r\n");

    printf("Request we're to write is as follows:\n%s\n", write_buf);

    /* Attempts to write len bytes from buf to BIO */ 
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}


/*
 * Function: request_azure_signin_code
 *-----------------------------------
 * *code: character buffer that will have the code inside of it by the function's end.
 *
 * *resource_id: char array containing MS resource id
 *
 * *clientid: contains client id of application as registered with Azure.
 *
 * *tenant: the MS tenant. 
 *
 * returns EXIT_FAILURE if the code buffer is empty at the end of the function
 * and EXIT_SUCCESS if the code buffer is anything but empty. 
 * 
 * TODO: Improve checking if this function succeeded. Should be some more error 
 * handling and there will need to be some way to log failures. 
*/

int request_azure_signin_code(char *user_code, const char *resource_id, const char *client_id, const char *tenant, char *device_code){
    char response_buf[2048];
    char code_buf[100];
    char json_buf[2048];
    cJSON *json;
    int start, end;

    read_code_from_microsoft(resource_id, client_id, tenant, response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    strcpy(user_code, cJSON_GetObjectItem(json, "user_code")->valuestring);
    strcpy(device_code, cJSON_GetObjectItem(json, "device_code")->valuestring);
    if (user_code[0] == '\0' || device_code[0] == '\0'){
        /* string is empty, we have failed somewhere */
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/* purely for testing, takes no command line args */
int main(int argc, char *argv[]){
    /* initialize variables */
    const char *resource_id;
    const char *client_id;
    const char *tenant; 
    char response_buf[160000];
    char code_buf[100];
    char json_buf[160000];
    char graph_buf[100000];
    cJSON *json; 
    char user_code[20];
    char device_code[1000];
    int resp;

    /* Provide hardcoded values for testing */
    resource_id = "00000002-0000-0000-c000-000000000000";
    client_id = "7262ee1e-6f52-4855-867c-727fc64b26d5";
    tenant = "digipirates.onmicrosoft.com";
    request_azure_signin_code(user_code, resource_id, client_id, tenant, device_code);
    int start, end;
    printf("user code is %s\n", user_code);
    printf("device code is %s\n", device_code);
    char key[1];
    puts("Press any key to continue...");
    getchar();
    resp = request_azure_oauth_token(device_code, resource_id, client_id, response_buf);
    if (resp == 1){
        printf("\nfailure...\n");
        printf("response buffer is %s\n", response_buf);
        return 1;
    }

    get_microsoft_graph_groups(response_buf, graph_buf);
    printf("The graph_response is %s\n", graph_buf);
    return 0;
}