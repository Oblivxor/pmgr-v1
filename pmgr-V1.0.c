#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ================= COLORS ================= */
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

/* ================= CONFIG ================= */
#define VAULT_FILE "vault.dat"
#define MAX_LINE 512

/* ================= BANNER ================= */
void banner() {
    printf(CYAN BOLD);
    printf(" ██████╗ ███╗   ███╗ ██████╗ ██████╗ \n");
    printf(" ██╔══██╗████╗ ████║██╔════╝ ██╔══██╗\n");
    printf(" ██████╔╝██╔████╔██║██║  ███╗██████╔╝\n");
    printf(" ██╔═══╝ ██║╚██╔╝██║██║   ██║██╔══██╗\n");
    printf(" ██║     ██║ ╚═╝ ██║╚██████╔╝██║  ██║\n");
    printf(" ╚═╝     ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═╝\n");
    printf("        Secure Password Manager (Phase 1)\n");
    printf(RESET "\n");
}

/* ================= ERROR ================= */
void die(const char *msg) {
    printf(RED "✖ %s\n" RESET, msg);
    exit(1);
}

/* ================= KEY DERIVATION ================= */
void derive_key(char *password, unsigned char *key) {
    unsigned char salt[crypto_pwhash_SALTBYTES];

    if (access(VAULT_FILE, F_OK) == 0) {
        FILE *f = fopen(VAULT_FILE, "rb");
        fread(salt, 1, sizeof salt, f);
        fclose(f);
    } else {
        randombytes_buf(salt, sizeof salt);
        FILE *f = fopen(VAULT_FILE, "wb");
        fwrite(salt, 1, sizeof salt, f);
        fclose(f);
    }

    if (crypto_pwhash(
        key,
        crypto_secretbox_KEYBYTES,
        password,
        strlen(password),
        salt,
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE,
        crypto_pwhash_ALG_DEFAULT) != 0) {
        die("Key derivation failed");
    }
}

/* ================= ADD ENTRY ================= */
void add_entry() {
    char master[128], service[64], user[64], pass[64];
    unsigned char key[crypto_secretbox_KEYBYTES];

    printf(YELLOW "› Master password: " RESET);
    fgets(master, sizeof master, stdin);
    master[strcspn(master, "\n")] = 0;

    derive_key(master, key);

    printf(YELLOW "› Service: " RESET);
    fgets(service, sizeof service, stdin);
    service[strcspn(service, "\n")] = 0;

    printf(YELLOW "› Username: " RESET);
    fgets(user, sizeof user, stdin);
    user[strcspn(user, "\n")] = 0;

    printf(YELLOW "› Password: " RESET);
    fgets(pass, sizeof pass, stdin);
    pass[strcspn(pass, "\n")] = 0;

    char plain[MAX_LINE];
    snprintf(plain, sizeof plain, "%s|%s|%s\n", service, user, pass);

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char cipher[MAX_LINE + crypto_secretbox_MACBYTES];
    randombytes_buf(nonce, sizeof nonce);

    crypto_secretbox_easy(cipher,
        (unsigned char *)plain,
        strlen(plain),
        nonce,
        key);

    FILE *f = fopen(VAULT_FILE, "ab");
    fwrite(nonce, 1, sizeof nonce, f);
    fwrite(cipher, 1, strlen(plain) + crypto_secretbox_MACBYTES, f);
    fclose(f);

    sodium_memzero(master, sizeof master);
    sodium_memzero(pass, sizeof pass);

    printf(GREEN "✔ Entry stored securely\n" RESET);
}

/* ================= GET ENTRY ================= */
void get_entry() {
    char master[128], target[64];
    unsigned char key[crypto_secretbox_KEYBYTES];

    printf(YELLOW "› Master password: " RESET);
    fgets(master, sizeof master, stdin);
    master[strcspn(master, "\n")] = 0;

    derive_key(master, key);

    printf(YELLOW "› Service name: " RESET);
    fgets(target, sizeof target, stdin);
    target[strcspn(target, "\n")] = 0;

    FILE *f = fopen(VAULT_FILE, "rb");
    if (!f) die("Vault not found");

    fseek(f, crypto_pwhash_SALTBYTES, SEEK_SET);

    while (!feof(f)) {
        unsigned char nonce[crypto_secretbox_NONCEBYTES];
        unsigned char cipher[MAX_LINE + crypto_secretbox_MACBYTES];
        char plain[MAX_LINE];

        if (fread(nonce, 1, sizeof nonce, f) != sizeof nonce) break;
        size_t r = fread(cipher, 1, sizeof cipher, f);

        if (crypto_secretbox_open_easy(
            (unsigned char *)plain,
            cipher,
            r,
            nonce,
            key) == 0) {

            char *svc = strtok(plain, "|");
            char *usr = strtok(NULL, "|");
            char *pwd = strtok(NULL, "\n");

            if (svc && strcmp(svc, target) == 0) {
                printf(GREEN "\nUsername: %s\nPassword: %s\n" RESET, usr, pwd);
                fclose(f);
                return;
            }
        }
    }

    fclose(f);
    printf(RED "✖ Entry not found\n" RESET);
}

/* ================= MAIN ================= */
int main(int argc, char **argv) {
    if (sodium_init() < 0) die("libsodium init failed");

    banner();

    if (argc != 2) {
        printf(BLUE "Usage:\n" RESET);
        printf("  %s add   → Add password\n", argv[0]);
        printf("  %s get   → Retrieve password\n\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "add") == 0)
        add_entry();
    else if (strcmp(argv[1], "get") == 0)
        get_entry();
    else
        printf(RED "Unknown command\n" RESET);

    return 0;
}
