#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "strmap.h"
#include <time.h>

#define BYTE_SIZE 3
#define TRUE 1
#define MAX_LENGTH 25
static const char DIGEST[] = "SHA256";

void hash(char *s, char *mess1);
void gen_random( char *s, const int len);
int compare_hex_case_sen(const char* a, const char* b);
void break_hash(char *msg);
void collision_break();

int main() {

	OpenSSL_add_all_algorithms();
	//break_hash("quick");
	collision_break();	
	getchar();
	return 1;
}

void collision_break() {
	StrMap *sm;
	char buf[255];
	int result;
	sm = sm_new(10);
	if (sm == NULL) {
	    return;
	}
	char *rand_msg = malloc(sizeof(char) * MAX_LENGTH + 1);
	char *hash_rand = malloc(sizeof(char) * BYTE_SIZE*2 + 1);
	srand(time(NULL));  

	int count = 0;	
	while(TRUE) { 
		count = count + 1;
		int r = rand() % MAX_LENGTH + 1;	
		gen_random(rand_msg, r);
		hash(hash_rand, rand_msg);
	
		result = sm_get(sm, hash_rand, buf, sizeof(buf));
		if (result == 0) {
			sm_put(sm, hash_rand, rand_msg);
		} else {
			if (compare_hex_case_sen(buf, rand_msg) == 1) 
				continue;

			printf("collision found in %d tries!\n", count);
			printf("first data string: %s, hash: %s\n", rand_msg, hash_rand);
			printf("second data string: %s, hash: %s\n", buf, hash_rand);			
			break;		
		}
	}

	sm_delete(sm);
}

void break_hash(char *msg) {
	char *hash_msg = malloc(sizeof(char) * BYTE_SIZE*2 + 1);
	hash(hash_msg, msg);
	printf("Message: %s, Hashed: %s \n", msg, hash_msg);	
	char *rand_msg = malloc(sizeof(char) * MAX_LENGTH + 1);
	char *hash_rand = malloc(sizeof(char) * BYTE_SIZE*2 + 1);	
	srand(time(NULL));  

	int count = 0;	
	while(TRUE) { 
		count = count + 1;
		//random int between 1 and 25
		int r = rand() % MAX_LENGTH + 1;	
		gen_random(rand_msg, r);
		hash(hash_rand, rand_msg);
		//printf("Random Message: %s, Random Hashed: %s \n", rand_msg, hash_rand);
		if (compare_hex_case_sen(hash_msg, hash_rand) == 1) {		
			printf("Random Message: %s, Random Hashed: %s \n", rand_msg, hash_rand);
			break;
		}
	}
	
	printf("\n\n FOUND IT IN %d tries!", count);
}

void hash(char * s, char *mess1) {
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	int md_len, i;

	md = EVP_get_digestbyname(DIGEST);
	if (md == NULL) {
		printf("Unknown message digest %s\n", DIGEST);
		exit(1);
	}

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, mess1, strlen(mess1));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);

	for(i=0;i<BYTE_SIZE;i++) {
		sprintf(&s[i*2], "%02X", md_value[i]);
	}
}

//Generate random string with size 
void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
	" ";
	int i;
    for (i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

int compare_hex_case_sen(const char* a, const char* b){
	unsigned int size1 = strlen(a);
	if (strlen(b) != size1)
		return 0;
	int i;
	for (i = 0; i < size1; i++)
		if (tolower(a[i]) != tolower(b[i]))
			return 0;
	return 1;
}
