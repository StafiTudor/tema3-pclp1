//314CAb-stafi Tudor Vasile
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *type;
	int savex, savey, h1, w1, h2, w2, intens;
} image;
//rp=red pixel, bp=blue pixel
typedef struct {
	unsigned char rp;
	unsigned char bp;
	unsigned char gp;
	unsigned char gr;
} matrix;

int clamp(int x)
{
	if (x > 255)
		return 255;
	if (x < 0)
		return 0;
	return x;
}

int invalid(char *com)
{
	// se verifica daca este valida sau nu comanda
	if (strncmp(com, "LOAD", 4) == 0 && strlen(com) > 5)
		return 1;
	if (strncmp(com, "APPLY", 5) == 0)
		return 1;
	if (strncmp(com, "CROP", 4) == 0)
		return 1;
	if (strncmp(com, "SAVE", 4) == 0)
		return 1;
	if (strncmp(com, "HISTOGRAM", 9) == 0)
		return 1;
	if (strncmp(com, "EQUALIZE", 8) == 0)
		return 1;
	if (strncmp(com, "SELECT", 6) == 0)
		return 1;
	if (strncmp(com, "EXIT", 4) == 0)
		return 1;
	if (strncmp(com, "ROTATE", 6) == 0)
		return 1;
	return 0;
}

void load(const char *filename, image *b, matrix ***mat)
{
	FILE *file;
	file = fopen(filename, "r");
	if (!file) {
		printf("Failed to load %s\n", filename);
	} else {
		char newline_char;
		printf("Loaded %s\n", filename);

		b->type = (char *)malloc(3 * sizeof(char));
		fscanf(file, "%s%d%d", b->type, &b->w2, &b->h2);
		fclose(file);
		b->type[2] = '\0';
		*mat = (matrix **)malloc(b->h2 * sizeof(matrix *));
		for (int i = 0; i < b->h2; i++)
			(*mat)[i] = (matrix *)malloc(b->w2 * sizeof(matrix));
		b->h1 = 0;
		b->w1 = 0;
		b->savex = b->h2;
		b->savey = b->w2;
		if (strncmp(b->type, "P2", 2) == 0 || strncmp(b->type, "P3", 2) == 0) {
			// Deschiderea fisierului în mod ASCII
			file = fopen(filename, "r");
			fscanf(file, "%s", b->type);
			fscanf(file, "%d%d%d", &b->w2, &b->h2, &b->intens);
			if (strncmp(b->type, "P2", 2) == 0) {
				for (int i = 0; i < b->h2; i++)
					for (int j = 0; j < b->w2; j++)
						fscanf(file, "%hhu", &(*mat)[i][j].gr);
			}
			if (strncmp(b->type, "P3", 2) == 0) {
				for (int i = 0; i < b->h2; i++)
					for (int j = 0; j < b->w2; j++) {
						fscanf(file, "%hhu", &(*mat)[i][j].rp);
						fscanf(file, "%hhu", &(*mat)[i][j].gp);
						fscanf(file, "%hhu", &(*mat)[i][j].bp);
					}
			}

		} else if (!(strncmp(b->type, "P5", 2) && strncmp(b->type, "P6", 2))) {
			// Deschiderea fisierului în mod binar
			file = fopen(filename, "rb");
			fscanf(file, "%s", b->type);
			fscanf(file, "%d%d%d", &b->w2, &b->h2, &b->intens);
			//caracter gol
			fread(&newline_char, sizeof(char), 1, file);
			// Citirea matricei din restul fisierului
			if (strncmp(b->type, "P5", 2) == 0) {
				for (int i = 0; i < b->h2; i++)
					for (int j = 0; j < b->w2; j++)
						fread(&(*mat)[i][j].gr, sizeof(unsigned char), 1, file);
			}

			if (strncmp(b->type, "P6", 2) == 0) {
				for (int i = 0; i < b->h2; i++)
					for (int j = 0; j < b->w2; j++) {
						fread(&(*mat)[i][j].rp, sizeof(unsigned char), 1, file);
						fread(&(*mat)[i][j].gp, sizeof(unsigned char), 1, file);
						fread(&(*mat)[i][j].bp, sizeof(unsigned char), 1, file);
					}
			}
		}

		fclose(file);
	}
}

void exitfct(matrix ***mat, image *b, int loaded)
{
	if (loaded) {
	// Eliberam memoria pentru fiecare linie a matricei
		for (int i = 0; i < b->savex; i++)
			free((*mat)[i]);
		free(b->type);
	// Eliberam memoria pentru pointerii la linii
		free(*mat);
	} else {
		printf("No image loaded\n");
	}
}

void loadfct(matrix ***mat, image *b, int *loaded)
{
	if (*loaded == 1) {
		for (int i = 0; i < b->savex; i++)
			free((*mat)[i]);
		free(*mat);
		free(b->type);
		*loaded = 0;
	}
}

void inter(int *x1, int *y1, int *x2, int *y2)
{
	if (*x1 > *x2) {
		int aux = *x1;
		*x1 = *x2;
		*x2 = aux;
	}
	if (*y1 > *y2) {
		int aux = *y1;
		*y1 = *y2;
		*y2 = aux;
	}
}

void save(const char *filename, image *b, matrix **mat, int mod, int loaded)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	FILE *file;
	if (mod == 0) {
		file = fopen(filename, "w"); // Deschidere în mod ASCII
		if (strncmp(b->type, "P2", 2) == 0 || strncmp(b->type, "P5", 2) == 0) {
			b->type[1] = '2';
			fprintf(file, "%s\n%d ", b->type, b->savey);
			fprintf(file, "%d\n%d\n", b->savex, b->intens);
			for (int i = 0; i < b->savex; i++) {
				for (int j = 0; j < b->savey; j++)
					fprintf(file, "%hhu ", mat[i][j].gr);
				fprintf(file, "\n");
			}
		}
		if (strncmp(b->type, "P3", 2) == 0 || strncmp(b->type, "P6", 2) == 0) {
			b->type[1] = '3';
			fprintf(file, "%s\n%d ", b->type, b->savey);
			fprintf(file, "%d\n%d\n", b->savex, b->intens);
			for (int i = 0; i < b->savex; i++) {
				for (int j = 0; j < b->savey; j++) {
					fprintf(file, "%hhu ", mat[i][j].rp);
					fprintf(file, "%hhu ", mat[i][j].gp);
					fprintf(file, "%hhu ", mat[i][j].bp);
				}
				fprintf(file, "\n");
			}
		}

	} else {
		file = fopen(filename, "wb"); // Deschidere în mod binar

		if (strncmp(b->type, "P2", 2) == 0 || strncmp(b->type, "P5", 2) == 0) {
			b->type[1] = '5';
			fprintf(file, "%s\n%d ", b->type, b->savey);
			fprintf(file, "%d\n%d\n", b->savex, b->intens);
			for (int i = 0; i < b->savex; i++)
				for (int j = 0; j < b->savey; j++)
					fwrite(&mat[i][j].gr, sizeof(unsigned char), 1, file);
		}
		if (strncmp(b->type, "P3", 2) == 0 || strncmp(b->type, "P6", 2) == 0) {
			b->type[1] = '6';
			fprintf(file, "%s\n%d ", b->type, b->savey);
			fprintf(file, "%d\n%d\n", b->savex, b->intens);
			for (int i = 0; i < b->savex; i++)
				for (int j = 0; j < b->savey; j++) {
					fwrite(&mat[i][j].rp, sizeof(unsigned char), 1, file);
					fwrite(&mat[i][j].gp, sizeof(unsigned char), 1, file);
					fwrite(&mat[i][j].bp, sizeof(unsigned char), 1, file);
				}
		}
	}

	fclose(file); // inchidere fisier

	printf("Saved %s\n", filename);
}

void crop(matrix ***mat, image *b, int loaded)
{
	if (loaded == 0) {
		printf("No image loaded\n");
	} else {
		printf("Image cropped\n");
		// Calculam dimensiunile noii submatrici
		int new_height = b->h2 - b->h1;
		int new_width = b->w2 - b->w1;
		// Alocam dinamic o matrice temporara pentru submatrice
		matrix **temp_matrix = (matrix **)malloc(new_height * sizeof(matrix *));
		for (int i = 0; i < new_height; i++)
			temp_matrix[i] = (matrix *)malloc(new_width * sizeof(matrix));
		// Copiem segmentul in matricea temporara
		for (int i = 0; i < new_height; i++)
			for (int j = 0; j < new_width; j++)
				temp_matrix[i][j] = (*mat)[b->h1 + i][b->w1 + j];
		// Eliberam memoria alocata pentru matricea initiala
		for (int i = 0; i < b->savex; i++)
			free((*mat)[i]);
		free(*mat);
		// Actualizam pointerul matricei si dimensiunile
		*mat = temp_matrix;
		b->h1 = 0;
		b->w1 = 0;
		b->h2 = new_height;
		b->w2 = new_width;
		b->savex = new_height;
		b->savey = new_width;
	}
}

void histogram(matrix **mat, image b, int stars, int bins, int loaded, char a[])
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	int nr1 = 0;
	int x = strlen(a) - 1;
	for (int i = 0; i <= x; i++)
		if (a[i] == ' ')
			nr1++;

	if (nr1 != 2) {
		printf("Invalid command\n");
		return;
	}
	if (strncmp(b.type, "P3", 2) == 0 || strncmp(b.type, "P6", 2) == 0) {
		printf("Black and white image needed\n");
		return;
	}
	int frecv1[260], nr = 256 / bins, frecv2[260];
	double stea = 0;
	for (int i = 0; i <= 257; i++)
		frecv1[i] = 0;
	for (int i = 0; i <= 257; i++)
		frecv2[i] = 0;
	for (int i = 0; i < b.h2; i++)
		for (int j = 0; j < b.w2; j++)
			frecv1[mat[i][j].gr]++;
	int max = -1;
	for (int i = 0; i < bins; i++) {
		for (int j = i * nr; j < nr * i + nr; j++)
			frecv2[i] = frecv2[i] + frecv1[j];
		if (max < frecv2[i])
			max = frecv2[i];
	}
	for (int i = 0; i < bins; i++) {
		stea = (frecv2[i] * stars) / max;
		printf("%d\t|\t", (int)stea);
		for (int j = 0; j < (int)stea; j++)
			printf("*");
		printf("\n");
	}
}

void equalize(matrix ***mat, image b, int loaded)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (strncmp(b.type, "P3", 2) == 0 || strncmp(b.type, "P6", 2) == 0) {
		printf("Black and white image needed\n");
		return;
	}

	int frecv[256], arie = b.savex * b.savey, s;
	// Alocam spatiu unei matrice temporare
	matrix **tmp = (matrix **)malloc(b.savex * sizeof(matrix *));
	for (int i = 0; i < b.savex; i++)
		tmp[i] = (matrix *)malloc(b.savey * sizeof(matrix));

	for (int i = 0; i < b.savex; i++)
		for (int j = 0; j < b.savey; j++)
			tmp[i][j] = (*mat)[i][j];

	for (int i = 0; i <= 255; i++)
		frecv[i] = 0;
	for (int i = 0; i < b.savex; i++)
		for (int j = 0; j < b.savey; j++)
			frecv[(*mat)[i][j].gr]++;
	double new = 0;
	for (int i = 0; i < b.savex; i++)
		for (int j = 0; j < b.savey; j++) {
			s = 0;
			for (int e = 0; e <= (*mat)[i][j].gr; e++)
				s = s + frecv[e];
			new = (255 * s) / arie;
			tmp[i][j].gr = round(new);
			tmp[i][j].gr = clamp(new);
		}
	// Eliberam memoria alocata pentru matricea initiala
	for (int i = 0; i < b.savex; i++)
		free((*mat)[i]);
	free(*mat);

	*mat = tmp;
	printf("Equalize done\n");
}

void edge(int ker[][3])
{
	int i, j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			if (i == 1 && j == 1)
				ker[i][j] = 8;
			else
				ker[i][j] = -1;
}

void sharpen(int ker[][3])
{
	ker[0][0] = 0;
	ker[0][2] = 0;
	ker[2][0] = 0;
	ker[2][2] = 0;
	ker[0][1] = -1;
	ker[2][1] = -1;
	ker[1][0] = -1;
	ker[1][2] = -1;
	ker[1][1] = 5;
}

void blur(int ker[][3])
{
	int i, j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			ker[i][j] = 1;
}

void gaussianblur(int ker[][3])
{
	ker[0][0] = 1;
	ker[0][2] = 1;
	ker[2][0] = 1;
	ker[2][2] = 1;
	ker[0][1] = 2;
	ker[2][1] = 2;
	ker[1][0] = 2;
	ker[1][2] = 2;
	ker[1][1] = 4;
}

int kernel(char *numek, int ker[][3], char *name)
{
	// Verificam sirul si apelam functia corespunzatoare
	if (strcmp(numek, "BLUR") == 0) {
		blur(ker);
		return 1;
	} else if (strcmp(numek, "EDGE") == 0) {
		edge(ker);
		return 2;
	} else if (strcmp(numek, "GAUSSIAN_BLUR") == 0) {
		gaussianblur(ker);
		return 3;
	} else if (strcmp(numek, "SHARPEN") == 0) {
		sharpen(ker);
		return 4;
	}
	if (strlen(name) <= 6)
		return 5;
	return 6;
}

void apply(matrix ***mat, image *b, int ker[][3], int tip, char *numek, int l)
{
	if (l == 0) {
		printf("No image loaded\n");
		return;
	}
	if (tip == 5) {
		printf("Invalid command\n");
		return;
	}
	if (tip == 6) {
		printf("APPLY parameter invalid\n");
		return;
	}
	if (strncmp(b->type, "P2", 2) == 0 || strncmp(b->type, "P5", 2) == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
		int x = b->savex - 1, y = b->savey - 1;
		int sr = 0, sg = 0, sb = 0;
		matrix **tmp = (matrix **)malloc(b->savex * sizeof(matrix *));
		for (int i = 0; i < b->savex; i++)
			tmp[i] = (matrix *)malloc(b->savey * sizeof(matrix));

		for (int i = 0; i < b->savex; i++)
			for (int j = 0; j < b->savey; j++)
				tmp[i][j] = (*mat)[i][j];
		for (int i = b->h1; i < b->h2; i++)
			for (int j = b->w1; j < b->w2; j++) {
				if (i != 0 && j != 0 && i != x && j != y) {
					int nrcol = 0, nrlin = 0;
					sr = 0;
					sg = 0;
					sb = 0;
				for (int e = i - 1; e <= i + 1; e++) {
					nrcol = 0;
					for (int h = j - 1; h <= j + 1; h++) {
						sr = sr + ((*mat)[e][h].rp * ker[nrlin][nrcol]);
						sg = sg + ((*mat)[e][h].gp * ker[nrlin][nrcol]);
						sb = sb + ((*mat)[e][h].bp * ker[nrlin][nrcol]);
						nrcol++;
					}
						nrlin++;
				}
					if (tip == 1) {
						sr = round((1.0 * sr) / 9);
						sg = round((1.0 * sg) / 9);
						sb = round((1.0 * sb) / 9);
					}
					if (tip == 3) {
						sr = round((1.0 * sr) / 16);
						sg = round((1.0 * sg) / 16);
						sb = round((1.0 * sb) / 16);
					}
					sr = clamp(sr);
					sg = clamp(sg);
					sb = clamp(sb);
					tmp[i][j].rp = sr;
					tmp[i][j].gp = sg;
					tmp[i][j].bp = sb;
				}
				// Eliberam memoria alocata pentru matricea initiala
			}

		for (int i = 0; i < b->savex; i++)
			free((*mat)[i]);
		free(*mat);

		*mat = tmp;

		printf("APPLY %s done\n", numek);
}

void selectie(image *b, int x1, int y1, int x2, int y2)
{
	if (x1 < 0 || x2 > b->savex || y1 < 0 || y2 > b->savey) {
		printf("Invalid set of coordinates\n");
	} else if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
	} else {
		b->h1 = x1;
		b->w1 = y1;
		b->h2 = x2;
		b->w2 = y2;
		printf("Selected %d %d ", b->w1, b->h1);
		printf("%d %d\n", b->w2, b->h2);
	}
}

void allsel(int loaded)
{
	if (loaded == 0)
		printf("No image loaded\n");
	else
		printf("Selected ALL\n");
}

int verif(char a[])
{
	if (a[0] == '\0')
		return 0;
	int x = strlen(a) - 1;
	for (int j = 1; j <= x; j++)
		if (!isdigit(a[j]))
			return 0;
	if (a[0] == '-' || isdigit(a[0]))
		return 1;
	return 1;
}

int verifunghi(int unghi)
{
	// Verificăm valorile unghiurilor
	if (unghi == -360 || unghi == 360 || unghi == 0)
		return 0;
	if (unghi == -90 || unghi == 270)
		return 1;
	if (unghi == 180 || unghi == -180)
		return 2;
	if (unghi == -270 || unghi == 90)
		return 3;
	return 4; // În caz contrar, returnăm 4
}

void rotate(matrix ***mat, image *b)
{
	int x = b->h1, y = b->w1, z = b->h2, t = b->w2;
	int aux;
	int new_width = b->h2 - b->h1;
	int new_height = b->w2 - b->w1;
	matrix **tmp = (matrix **)malloc(new_height * sizeof(matrix *));
	for (int i = 0; i < new_height; i++)
		tmp[i] = (matrix *)malloc(new_width * sizeof(matrix));

	for (int i = 0; i < new_width; i++)
		for (int j = 0; j < new_height; j++)
			tmp[new_height - 1 - j][i] = (*mat)[i + b->h1][j + b->w1];
	if (!(x == 0 && y == 0 && z == b->savex && t == b->savey)) {
		for (int i = 0; i < new_width; i++)
			for (int j = 0; j < new_height; j++)
				(*mat)[i + b->h1][j + b->w1] = tmp[i][j];
		for (int i = 0; i < new_height; i++)
			free(tmp[i]);
		free(tmp);
	} else {
		for (int i = 0; i < b->savex; i++)
			free((*mat)[i]);
		free(*mat);
		(*mat) = tmp;
		b->savex = new_height; // height
		b->savey = new_width;	 // width
		aux = b->h1;
		b->h1 = b->w1;
		b->w1 = aux;
		aux = b->h2;
		b->h2 = b->w2;
		b->w2 = aux;
	}
}

void supported(matrix ***mat, image *b, int unghi, int loaded, int m, char s[])
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (strlen(s) <= 7) {
		printf("Invalid command\n");
		return;
	}
	if (m == 4) {
		printf("Unsupported rotation angle\n");
		return;
	}
	int x = b->h1, y = b->w1, z = b->h2, t = b->w2;
	if (!(x == 0 && y == 0 && z == b->savex && t == b->savey))
		if ((z - x) != (t - y)) {
			printf("The selection must be square\n");
			return;
		}
	for (int i = 0; i < m; i++)
		rotate(mat, b);

	printf("Rotated %d\n", unghi);
}

int main(void)
{
	char com[100], fis[100], numek[100]; matrix **mat;
	char x1[30], y1[30], x2[30], y2[30];
	int loaded = 0, stars, bins, ker[3][3], ok, unghi = -1, vf;
	x1[0] = '\0'; x2[0] = '\0'; y1[0] = '\0'; y2[0] = '\0';
	image b; fis[0] = '\0'; numek[0] = '\0';
	fgets(com, sizeof(com), stdin);
	if (com[strlen(com) - 1] == '\n')
		com[strlen(com) - 1] = '\0';
	while (strcmp(com, "EXIT")) {
		if (invalid(com) == 0)
			printf("Invalid command\n");
		if (strncmp(com, "LOAD ", 5) == 0) {
			ok = sscanf(com, "LOAD %s", fis); FILE *file = fopen(fis, "r");
			if (!file) {
				if (strlen(fis))
					printf("Failed to load %s\n", fis);
				loadfct(&mat, &b, &loaded);
			} else {
				fclose(file); load(fis, &b, &mat); loaded = 1;
			}
		fis[0] = '\0';
		}
		if (strncmp(com, "SELECT ALL", 10) == 0) {
			b.h1 = 0; b.w1 = 0; b.h2 = b.savex; b.w2 = b.savey; allsel(loaded);
		} else if (strncmp(com, "SELECT", 6) == 0) {
			if (loaded == 1) {
				ok = sscanf(com, "SELECT %s %s %s %s", y1, x1, y2, x2);
				if (!verif(y1) || !verif(x1) || !verif(y2) || !verif(x2)) {
					printf("Invalid command\n");
				} else {
					int p1, p2, p3, p4;
					ok = sscanf(com, "SELECT %d %d %d %d", &p1, &p2, &p3, &p4);
					inter(&p2, &p1, &p4, &p3); selectie(&b, p2, p1, p4, p3);
					x1[0] = '\0'; x2[0] = '\0'; y1[0] = '\0'; y2[0] = '\0';
				}
			} else {
				printf("No image loaded\n");
			}
		}
		if (strncmp(com, "SAVE", 4) == 0) {
			int mod; char mode[10];
			ok = sscanf(com, "SAVE %s %s", fis, mode);
			if (ok == 2 || ok == 1) {
				if (strstr(com, "ascii"))
					mod = 0; // 0 pt ascii
				else
					mod = 1; // 1 pt binar
			save(fis, &b, mat, mod, loaded);
			} else if (loaded) {
				printf("Invalid command\n");
			} else {
				printf("No image loaded\n");
			}
		}
		if (strncmp(com, "CROP", 4) == 0)
			crop(&mat, &b, loaded);
		if (strncmp(com, "HISTOGRAM", 9) == 0) {
			ok = sscanf(com, "HISTOGRAM %d %d", &stars, &bins);
			histogram(mat, b, stars, bins, loaded, com);
		}
		if (strncmp(com, "EQUALIZE", 8) == 0)
			equalize(&mat, b, loaded);
		if (strncmp(com, "APPLY", 5) == 0) {
			ok = sscanf(com, "APPLY %s", numek);
				int tip_apply = kernel(numek, ker, com);
				apply(&mat, &b, ker, tip_apply, numek, loaded);
		}
		if (strncmp(com, "ROTATE", 6) == 0) {
			ok = sscanf(com, "ROTATE %d", &unghi); vf = verifunghi(unghi);
			supported(&mat, &b, unghi, loaded, vf, com);
		}
		fgets(com, sizeof(com), stdin);
		if (com[strlen(com) - 1] == '\n')
			com[strlen(com) - 1] = '\0';
	}

	if (strcmp(com, "EXIT") == 0)
		exitfct(&mat, &b, loaded);
	return 0;
}
