#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct { int azonosito; char varosnev[30]; } beolvasott;
typedef struct { int azonosito; char varosnev[30]; double dist; int megvan; int honnan; } varos;
typedef struct { int varos1; int varos2; double tavolsag; } utak;
typedef struct lista { utak ut; struct lista *next; } lista;
typedef struct utvonal { int honnan; int mostani; struct utvonal *next; } utvonal;

int beolvastxt(lista *head)
{
	FILE *fp = fopen("utak.txt", "r");
	lista *p;
	p = head;

	if (fp == NULL)
	{
		printf("Hianyzik az utak.txt fajl");
		return 1;
	}

	while (fscanf(fp, "%d %d %lf", &(p->ut.varos1), &(p->ut.varos2), &(p->ut.tavolsag)) == 3)
	{
		p->next = (lista*)malloc(sizeof(lista));
		p = p->next;
	}
	p->next = NULL;

	p = head;
	while (p->next->next != NULL) p = p->next;
	free(p->next);
	p->next = NULL; //utolsónak nem jó adatokat olvas be, ezért a legutolsó elemet töröljük a láncolt listából
	fclose(fp);
	return 0;
}

int beolvasbin(varos varosok[], int *varosdb)
{
	FILE *fp = fopen("varosok.dat", "rb");
	int i = 0;
	beolvasott ideiglenes;
	if (fp == NULL)
	{
		printf("Hianyzik a varosok.dat fajl");
		return 1;
	}
	while (fread(&ideiglenes, sizeof(beolvasott), 1, fp)==1)
	{
		varosok[i].azonosito = ideiglenes.azonosito;
		strcpy(varosok[i].varosnev, ideiglenes.varosnev);
		i++;
	}
	*varosdb = i;
	fclose(fp);
	return 0;
}

int beolvas(varos varosok[], int varosdb, int *varos1, int *varos2)
{
	printf("Ird be a kiindulopontot es a celpontot, ekezet nelkul, szokozzel elvalasztva!\n");

	int i = 0;
	char v1[30], v2[30];
	if (scanf("%s %s", v1, v2) != 2) return 1;
	*varos1 = *varos2 = -1;
	for (i = 0; i < varosdb; i++)
	{
		if (strcmp(varosok[i].varosnev, v1) == 0) *varos1 = varosok[i].azonosito;
		if (strcmp(varosok[i].varosnev, v2) == 0) *varos2 = varosok[i].azonosito;
	}
	if (*varos1 == -1 || *varos2 == -1)
	{
		printf("Nem jo varosokat adtal meg\n");
		return 1;
	}
	return 0;
}

void beallit(varos varosok[], int varosdb, int kiindulopont)
{
	int i;
	for (i = 0; i < varosdb; i++)
	{
		if (varosok[i].azonosito == kiindulopont)
		{
			varosok[i].megvan = 1;
			varosok[i].dist = 0;
			varosok[i].honnan = -1;
		}
		else 
		{
			varosok[i].megvan = 0;
			varosok[i].dist = 10000;
		}
	}
}

void dijkstra(varos varosok[], int varosdb, int kiindulopont, int celpont, lista *head)
{
	int legutolso = kiindulopont;
	double legkisebb;
	lista *p = head;
	int i = 0;
	while (varosok[celpont].megvan==0)
	{
		for (p = head; p !=NULL; p=p->next)
		{
			if (p->ut.varos1 == legutolso)
			{
				if (varosok[legutolso].dist + (p->ut.tavolsag) < varosok[p->ut.varos2].dist)
				{
					varosok[p->ut.varos2].dist = varosok[legutolso].dist + (p->ut.tavolsag);
					varosok[p->ut.varos2].honnan = legutolso;
				}
			}
			if (p->ut.varos2 == legutolso)
			{
				if (varosok[legutolso].dist + (p->ut.tavolsag) < varosok[p->ut.varos1].dist)
				{
					varosok[p->ut.varos1].dist = varosok[legutolso].dist + (p->ut.tavolsag);
					varosok[p->ut.varos1].honnan = legutolso;
				}
			}
		}
		legkisebb = 10000;
		for (i = 0; i < varosdb;i++)
		{
			if (varosok[i].megvan == 0)
			{
				if (varosok[i].dist < legkisebb)
				{
					legkisebb = (varosok[i].dist);
					legutolso = (varosok[i].azonosito);
				}
			}
		}
		varosok[legutolso].megvan = 1;
	}
}

utvonal* utvonalterv(utvonal *head, int kiindulopont, int celpont, varos varosok[])
{
	utvonal *p;
	int mostanivaros = celpont;
	head->next = NULL;
	head->honnan = varosok[mostanivaros].honnan;
	head->mostani = celpont;
	while (varosok[mostanivaros].honnan != -1)
	{
		p = (utvonal*)malloc(sizeof(utvonal));
		p->next = head;
		mostanivaros = varosok[mostanivaros].honnan;
		p->honnan = varosok[mostanivaros].honnan;
		p->mostani = mostanivaros;
		head = p;
	}
	return head;
}

double szakaszhossz(int honnan, int hova, lista *head)
{
	lista *p;
	for (p = head; p != NULL; p = p->next)
	{
		if ((p->ut.varos1 == honnan && p->ut.varos2 == hova) || (p->ut.varos1 == hova && p->ut.varos2 == honnan))
		{
			return p->ut.tavolsag;
		}
	}
}

void kiir(utvonal *eleje, varos varosok[], int kiindulopont, int celpont, lista *head)
{
	utvonal *p;
	for (p = eleje->next; p != NULL; p = p->next)
	{
		printf("%s ----> %s", varosok[p->honnan].varosnev, varosok[p->mostani].varosnev);
		printf("\t%g km\n", szakaszhossz(p->honnan, p->mostani, head));
	}
	printf("Osszesen: %g km\n", varosok[celpont].dist);
}

void listatorol(lista *head, utvonal *eleje)
{
	lista *p = head;
	utvonal *r = eleje;

	while (head != NULL)
	{
		head = head->next;
		free(p);
		p = head;
	}
	free(head);

	while (eleje != NULL)
	{
		eleje = eleje->next;
		free(r);
		r = eleje;
	}
	free(eleje);
}

int main()
{
	varos varosok[100];
	lista *head = (lista*)malloc(sizeof(lista));
	utvonal *eleje = (utvonal*)malloc(sizeof(utvonal));
	int kiindulopont, celpont;
	int varosdb;

	if (beolvastxt(head) == 1) return 1;
	if (beolvasbin(varosok, &varosdb) == 1) return 1;
	if (beolvas(varosok, varosdb, &kiindulopont, &celpont) == 1) return 1;

	beallit(varosok, varosdb, kiindulopont);

	dijkstra(varosok, varosdb, kiindulopont, celpont, head);

	eleje = utvonalterv(eleje, kiindulopont, celpont, varosok);

	kiir(eleje, varosok, kiindulopont, celpont, head);

	listatorol(head, eleje);

	return 0;
}