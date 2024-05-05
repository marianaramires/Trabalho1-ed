#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define SEED 0x12345678

typedef struct
{
    uintptr_t *table;
    int size;
    int max;
    uintptr_t deleted;
    char *(*get_key)(void *);
} thash;

typedef struct _municipio
{
    char codigo_ibge[10];
    char nome[40];
    float latitude;
    float longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char fuso_horario[40];
} tmunicipio;

uint32_t hashf(const char *str, uint32_t h)
{
    /* One-byte-at-a-time Murmur hash
    Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp */
    for (; *str; ++str)
    {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

int hash_insere(thash *h, void *bucket)
{
    uint32_t hash = hashf(h->get_key(bucket), SEED);
    int pos = hash % (h->max);
    /*se esta cheio*/
    if (h->max == (h->size + 1))
    {
        free(bucket);
        return EXIT_FAILURE;
    }
    else
    { /*fazer a insercao*/
        while (h->table[pos] != 0)
        {
            if (h->table[pos] == h->deleted)
                break;
            pos = (pos + 1) % h->max;
        }
        h->table[pos] = (uintptr_t)bucket;
        h->size += 1;
    }
    return EXIT_SUCCESS;
}

int hash_constroi(thash *h, int nbuckets, char *(*get_key)(void *))
{
    h->table = calloc(sizeof(void *), nbuckets + 1);
    if (h->table == NULL)
    {
        return EXIT_FAILURE;
    }
    h->max = nbuckets + 1;
    h->size = 0;
    h->deleted = (uintptr_t) & (h->size);
    h->get_key = get_key;
    return EXIT_SUCCESS;
}

void *hash_busca(thash h, const char *key)
{
    int pos = hashf(key, SEED) % (h.max);
    void *ret = NULL;
    while (h.table[pos] != 0 && ret == NULL)
    {
        if (strcmp(h.get_key((void *)h.table[pos]), key) == 0)
        {
            ret = (void *)h.table[pos];
        }
        else
        {
            pos = (pos + 1) % h.max;
        }
    }
    return ret;
}

int hash_remove(thash *h, const char *key)
{
    int pos = hashf(key, SEED) % (h->max);
    while (h->table[pos] != 0)
    {
        if (strcmp(h->get_key((void *)h->table[pos]), key) == 0)
        { /* se achei remove*/
            free((void *)h->table[pos]);
            h->table[pos] = h->deleted;
            h->size -= 1;
            return EXIT_SUCCESS;
        }
        else
        {
            pos = (pos + 1) % h->max;
        }
    }
    return EXIT_FAILURE;
}

void hash_apaga(thash *h)
{
    int pos;
    for (pos = 0; pos < h->max; pos++)
    {
        if (h->table[pos] != 0)
        {
            if (h->table[pos] != h->deleted)
            {
                free((void *)h->table[pos]);
            }
        }
    }
    free(h->table);
}

char* trata(char *string){
    char *pt = strstr(string, ":");
    pt=pt+2;
    char *virgula = strchr(pt, ',');
    *virgula = '\0';

    return pt;
}

int main()
{
    char c[500];
    char t;
    int i=0;
    FILE *arq;
    arq = fopen("municipios.json", "r");
    tmunicipio teste;

    fgets(c, 100, arq); // lixo
    //while (!feof(arq))
    //{
        fgets(c, 100, arq); // lixo

        // codigo_ibge
        fgets(c, 500, arq); 
        strcpy(teste.codigo_ibge, trata(c));
        printf("%s\n", teste.codigo_ibge);

        // nome
        fgets(c, 500, arq); 
        strcpy(teste.nome, trata(c));
        printf("%s\n", teste.nome);

        // latitude
        fgets(c, 500, arq); 
        teste.latitude = atof(trata(c));
        printf("%f\n", teste.latitude);

        fgets(c, 100, arq); // lixo
    //}

    fclose(arq);

    return EXIT_SUCCESS;
}