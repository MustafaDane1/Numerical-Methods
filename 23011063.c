#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

// Denklem ifadesini token'lara ayýran fonksiyon
int tokenizeDenklem(const char *ifade, char *tokenler[], int maksTokenSayisi) {
    int i = 0, tokenSayisi = 0;
    while (ifade[i] != '\0') {
        if (!isspace(ifade[i])) {
            // Sayýsal deðerler (tam sayý veya ondalýklý)
            if (isdigit(ifade[i]) || ifade[i] == '.') {
                int baslangic = i;
                while (isdigit(ifade[i]) || ifade[i] == '.') i++;
                int uzunluk = i - baslangic;
                tokenler[tokenSayisi] = (char*)malloc((uzunluk + 1) * sizeof(char));
                strncpy(tokenler[tokenSayisi], ifade + baslangic, uzunluk);
                tokenler[tokenSayisi][uzunluk] = '\0';
                tokenSayisi++;
            }
            // Deðiþkenler ve fonksiyon isimleri
            else if (isalpha(ifade[i])) {
                int baslangic = i;
                while (isalpha(ifade[i]) || isdigit(ifade[i]) || ifade[i] == '_') i++;
                int uzunluk = i - baslangic;
                tokenler[tokenSayisi] = (char*)malloc((uzunluk + 1) * sizeof(char));
                strncpy(tokenler[tokenSayisi], ifade + baslangic, uzunluk);
                tokenler[tokenSayisi][uzunluk] = '\0';
                tokenSayisi++;
            }
            // Operatörler ve parantezler
            else {
                tokenler[tokenSayisi] = (char*)malloc(2 * sizeof(char));
                tokenler[tokenSayisi][0] = ifade[i];
                tokenler[tokenSayisi][1] = '\0';
                i++; //Operatör veya parantez ise i'yi burada artýr
                tokenSayisi++;
            }
            if (tokenSayisi >= maksTokenSayisi) {
                // Hata durumunda belleði temizle
                int j;
                for (j = 0; j < tokenSayisi; j++) {
                    free(tokenler[j]);
                    tokenler[j] = NULL;
                }
                fprintf(stderr, "Cok fazla token\n");
                return -1;
            }
        } else {
             i++; //Boþluk karakteri ise i'yi burada artýr
        }
        }
    return tokenSayisi;
}
// Operatör önceliðini döndüren fonksiyon
int operatorOnceligi(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 0;
}

//Shunting Yard algoritmasý ile infix ifadeyi postfix'e çevirme
int shuntingYard(char *infix[], int infixUzunluk, char *postfix[]) {
    char *stack[256];
    int stackTop = -1, postfixIndex = 0;
    int i;
    for (i = 0; i < infixUzunluk; i++) {
        // Sayý veya deðiþken ise doðrudan postfix'e ekle
        if (isdigit(infix[i][0]) || (infix[i][0] == '.' && isdigit(infix[i][1]))) {
            postfix[postfixIndex++] = strdup(infix[i]);
        }
        else if (strcmp(infix[i], "x") == 0 || strcmp(infix[i], "e") == 0) {
            postfix[postfixIndex++] = strdup(infix[i]);
        }
        else if (isalpha(infix[i][0]) && strcmp(infix[i], "x") != 0 && strcmp(infix[i], "e") != 0) {
            stack[++stackTop] = infix[i];
        }
         else if (infix[i][0] == '+' || infix[i][0] == '-' || infix[i][0] == '*' || infix[i][0] == '/' || infix[i][0] == '^') {
            // Üs operatörü için saðdan sola birleþme
            if (strcmp(infix[i], "^") == 0) {
                while (stackTop >= 0 && operatorOnceligi(stack[stackTop]) > operatorOnceligi(infix[i])) {
                postfix[postfixIndex++] = strdup(stack[stackTop--]);
                }
            } else {
                // Diðer operatörler için soldan saða birleþme
                while (stackTop >= 0 && operatorOnceligi(stack[stackTop]) >= operatorOnceligi(infix[i])) {
                    postfix[postfixIndex++] = strdup(stack[stackTop--]);
                }
            }
            stack[++stackTop] = infix[i];
        }
        else if (infix[i][0] == '(') {
            stack[++stackTop] = infix[i];
        }
        else if (infix[i][0] == ')') {
           while (stackTop >= 0 && stack[stackTop][0] != '(') {

				postfix[postfixIndex++] = strdup(stack[stackTop--]);
            }
            stackTop--;
            if (stackTop >= 0 && isalpha(stack[stackTop][0])) {
                postfix[postfixIndex++] = strdup(stack[stackTop--]);
            }
        }
    }
    // Stack'te kalan operatörleri postfix'e ekle
    while (stackTop >= 0) {
        postfix[postfixIndex++] = strdup(stack[stackTop--]);
    }

     return postfixIndex;
}

// Postfix ifadeyi deðerlendirme
double postfixDegerlendir(char *postfix[], int postfixUzunluk, double xDegeri) {
    double stack[256];
    int stackTop = -1;
    int i;

    for (i = 0; i < postfixUzunluk; i++) {
        // Sayi ise stacck'e ekle
        if (isdigit(postfix[i][0]) || (postfix[i][0] == '.' && isdigit(postfix[i][1]))) {
            stack[++stackTop] = strtod(postfix[i], NULL);
        }
        // Deðiþken(x) ise deðerini stack'e ekle
        else if (strcmp(postfix[i], "x") == 0) {
            stack[++stackTop] = xDegeri;
        }
        // Euler sayýsý (e) için
        else if (strcmp(postfix[i], "e") == 0) {
            stack[++stackTop] = 2.71828182845904523536; // Euler sayýsý
        }
        // Operatör ise iþlem yap
        else if (strcmp(postfix[i], "+") == 0) {
            double b = stack[stackTop--];
            double a = stack[stackTop--];
            stack[++stackTop] = a + b;
        }
        else if (strcmp(postfix[i], "-") == 0) {
            double b = stack[stackTop--];
            double a = stack[stackTop--];
            stack[++stackTop] = a - b;
        }
        else if (strcmp(postfix[i], "*") == 0) {
            double b = stack[stackTop--];
            double a = stack[stackTop--];
            stack[++stackTop] = a * b;
        }
        else if (strcmp(postfix[i], "/") == 0) {
            double b = stack[stackTop--];
            double a = stack[stackTop--];
            if (b == 0) {
                fprintf(stderr, "Sifira bolme hatasi!\n");
                return NAN;
            }
            stack[++stackTop] = a / b;
        }
        else if (strcmp(postfix[i], "^") == 0) {
             double b = stack[stackTop--];
             double a = stack[stackTop--];
            stack[++stackTop] = pow(a, b);
        }
        else if (strcmp(postfix[i], "sin") == 0) {
            double a = stack[stackTop--];
            stack[++stackTop] = sin(a);
        }
        else if (strcmp(postfix[i], "cos") == 0) {
            double a = stack[stackTop--];
            stack[++stackTop] = cos(a);
        }
        else if (strcmp(postfix[i], "tan") == 0) {
            double a = stack[stackTop--];
            stack[++stackTop] = tan(a);
        }
        else if (strcmp(postfix[i], "arcsin") == 0) {
             double a = stack[stackTop--];
            stack[++stackTop] = asin(a);
        }
        else if (strcmp(postfix[i], "arccos") == 0) {
             double a = stack[stackTop--];
            stack[++stackTop] = acos(a);
        }
        else if (strcmp(postfix[i], "arctan") == 0) {
             double a = stack[stackTop--];
            stack[++stackTop] = atan(a);
        }
        else if (strcmp(postfix[i], "arccot") == 0) {
            double a = stack[stackTop--];
            if (fabs(a) < 1e-9) {     //Sýfýra bölme hatasýný engelle
                fprintf(stderr, "acot(0) tanimsizdir.\n");
                return NAN;
            }
            stack[++stackTop] = atan(1.0/a);
        }
        else if (strncmp(postfix[i], "log_", 4) == 0) { // log_b(x) formatý, örn: log_5
           double a = stack[stackTop--];
            char *endptr;
           double base = strtod(postfix[i] + 4, &endptr);

            if (postfix[i] + 4 == endptr || *endptr != '\0') {
                fprintf(stderr, "Gecersiz log tabani formatinda: %s\n", postfix[i]);
                return NAN;
            }
            if (base <= 0 || fabs(base - 1.0) < 1e-9) {
            fprintf(stderr, "Logaritma tabani pozitif ve 1'den farkli olmalidir: %f\n", base);
                return NAN;
            }
            if (a <= 0) {
                   fprintf(stderr, "log_b(a) icin 'a' argumani pozitif olmalidir: %f\n", a);
                 return NAN;
            }
            stack[++stackTop] = log(a) / log(base);
        }
        else if (strcmp(postfix[i], "log") == 0) {
            double a = stack[stackTop--];
            if (a <= 0) {
                 fprintf(stderr, "log10(a) icin 'a' argumani pozitif olmalidir: %f\n", a);
                 return NAN;
            }
            stack[++stackTop] = log10(a);
        }
        else if (strcmp(postfix[i], "ln") == 0) {
            double a = stack[stackTop--];
            if (a <= 0) {
                 fprintf(stderr, "ln(a) icin 'a' argumani pozitif olmalidir: %f\n", a);
                 return NAN;
            }
            stack[++stackTop] = log(a);
        }


        else if (strcmp(postfix[i], "exp") == 0) {
        double a = stack[stackTop--];
               stack[++stackTop] = exp(a);
        }
        else {
             fprintf(stderr, "Bilinmeyen token: %s\n", postfix[i]);
               return NAN;
        }
    }

    return stack[stackTop];
}

// Denklem almak için fonksiyon
void denklemAl(char denklem[], int boyut) {
    printf("Denklemi giriniz: ");
    fflush(stdin); // Buffer temizleme
    fgets(denklem, boyut, stdin);
    int i = 0;
    while(denklem[i] != '\n' && denklem[i] != '\0') {
        i++;
    }
    if(denklem[i] == '\n') {
        denklem[i] = '\0';
    }
}


//Denklemi tokenize edip postfix formunda yazdýran fonksiyon
void denklemYazdir(const char *denklem) {
     char *tokenler[256];
     int tokenSayisi =  tokenizeDenklem(denklem, tokenler, 256);

    if (tokenSayisi <= 0) {
        printf("Denklem tokenize edilemedi!\n");
        return; }
     char *postfix[256];
     int  postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);

    if (postfixUzunluk > 0) {
        printf ("\nPostfix ifade: ");
        int j;
        for (j = 0; j <   postfixUzunluk; j++) {
            printf("%s ", postfix[j]);
        }
        printf("\n");

        // Postfix belleðini temizle
         for (j = 0; j < postfixUzunluk; j++) {
            free(postfix[j]);}
    } else {
        printf("Denklem postfix'e cevrilemedi!\n");
    }

    // Token belleðini temizle
    int i;
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
        }
}

float bisectionYontemi(const char *denklem) {
    double x1, x2, hata, x3, fx1, fx2, fx3, x3_onceki;
    int iterasyon = 0, maksIterasyon;
    int converged = 0;

    printf("\nBisection yontemi icin parametreleri girin:\n");
    printf("x1: ");
    scanf("%lf", &x1);
    printf("x2: ");
    scanf("%lf", &x2);
    printf("Hata toleransi: ");
    scanf("%lf", &hata);
    printf("Maksimum iterasyon sayisi: ");
    scanf("%d", &maksIterasyon);

    // Denklemi tokenize et ve postfix'e çevir
    char *tokenler[256];
    int tokenSayisi = tokenizeDenklem(denklem, tokenler, 256);
    char *postfix[256];
    int postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);
    fx1 = postfixDegerlendir(postfix, postfixUzunluk, x1);
    fx2 = postfixDegerlendir(postfix, postfixUzunluk, x2);
    printf("\nIterasyon\tx1\t\tx2\t\tx3\t\tf(x3)\n");
    printf("------------------------------------------\n");

    x3 = (x1 + x2) / 2.0;
    fx3 = postfixDegerlendir(postfix, postfixUzunluk, x3);
    x3_onceki = x3 + 2*hata; // Baþlangýçta farkýn büyük olmasý için
    while (iterasyon < maksIterasyon && fabs(x3 - x3_onceki) >= hata) { // Durma koþulu x3'lerin farkýna göre
        printf("%d\t%f\t%f\t%f\t%f\n", iterasyon + 1, x1, x2, x3, fx3);
        x3_onceki = x3;

        if (fx3 * fx1 < 0) {
            x2 = x3;
            fx2 = fx3;
        } else {
            x1 = x3;
            fx1 = fx3;
        }
        iterasyon++;
        // Yeni x3 ve fx3 deðerlerini hesapla
        x3 = (x1 + x2) / 2.0;
        fx3 = postfixDegerlendir(postfix, postfixUzunluk, x3);
      }

    // Son iterasyonun sonucunu yazdýr
    if ((iterasyon < maksIterasyon) && (fabs(x3 - x3_onceki) < hata )) {
         printf("%d\t%f\t%f\t%f\t%f\n", iterasyon + 1, x1, x2, x3, fx3);
      }
     if (fabs(x3 - x3_onceki) < hata) { // Durma koþulu x3'lerin farkýna göre
        printf("\nBulunan kok: %f\n", x3);
        converged = 1;
     }
    if (iterasyon >= maksIterasyon && !converged) {
        printf("Maksimum iterasyon sayisina ulasildi. Son yaklasim: %f\n", x3);
    }
    // Bellek temizlik
    int i;
    for (i = 0; i < postfixUzunluk; i++) {
        free(postfix[i]);
    }
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
    }
    return x3;
}

float regulaFalsiYontemi(const char *denklem) {
    double x1, x2, hata, x3, fx1, fx2, fx3, x3_onceki;
    int iterasyon = 0, maksIterasyon;
    int converged = 0;

    printf("\nRegula-Falsi yontemi icin parametreleri girin:\n");
    printf("x1: ");
    scanf("%lf", &x1);
    printf("x2: ");
    scanf("%lf", &x2);
    printf("Hata toleransi: ");
    scanf("%lf", &hata);
    printf("Maksimum iterasyon sayisi: ");
    scanf("%d", &maksIterasyon);

    // Denklemi tokenize et ve postfix'e çevr.
    char *tokenler[256];
    int tokenSayisi = tokenizeDenklem(denklem, tokenler, 256);
    char *postfix[256];
    int postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);

    fx1 = postfixDegerlendir(postfix, postfixUzunluk, x1);
    fx2 = postfixDegerlendir(postfix, postfixUzunluk, x2);
    printf("\nIterasyon\tx1\t\tx2\t\tx3\t\tf(x3)\n");
    printf("------------------------------------------\n");

    x3 =  x2 - (fx2 * (x2 - x1) / (fx2 - fx1));
    fx3 = postfixDegerlendir(postfix, postfixUzunluk, x3);
    x3_onceki = x3 + 2*hata;

    while  (iterasyon < maksIterasyon && fabs(x3 - x3_onceki) >= hata) {
        printf("%d\t%f\t%f\t%f\t%f\n", iterasyon + 1, x1, x2, x3, fx3);
        x3_onceki = x3;

        if (fx3 * fx1 <= 0) {
            x2 = x3;
            fx2 = fx3;
        } else {
            x1 = x3;
            fx1 = fx3;
        }
        iterasyon++;

        x3 = x2 - (fx2 * (x2 - x1) / (fx2 - fx1));
        fx3 = postfixDegerlendir(postfix, postfixUzunluk, x3);
    }

    if ((iterasyon < maksIterasyon) && (fabs(x3 - x3_onceki) < hata)) {
         printf("%d\t%f\t%f\t%f\t%f\n", iterasyon + 1, x1, x2, x3, fx3);
    }

    if (fabs(x3 - x3_onceki) < hata) {
        printf("\nBulunan kok: %f\n", x3);
        converged = 1;
    }

	if (iterasyon >= maksIterasyon && !converged) {
        printf("Maksimum iterasyon sayisina ulasildi. Son yaklasim: %f\n", x3);
    }
    // Bellek temizliði.
    int i;
    for (i = 0; i < postfixUzunluk; i++) {
        free(postfix[i]);
    }
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
    }
    return x3;
}

float newtonRaphsonYontemi(const char *denklem) {
    double x0, hata, x1 = NAN, fx0 = NAN, dfx0 = NAN;
    int iterasyon = 0, maksIterasyon;
    int converged = 0;

    printf("\nNewton-Raphson yontemi icin parametreleri girin:\n");
    printf("Baslangic noktasi (x0): ");
    scanf("%lf", &x0);
    printf("Hata toleransi: ");
    scanf("%lf", &hata);
    printf("Maksimum iterasyon sayisi: ");
    scanf("%d", &maksIterasyon);

    char *tokenler[256];
    int tokenSayisi = tokenizeDenklem(denklem, tokenler, 256);
    char *postfix[256];
    int postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);

    printf("\nIterasyon\tx0\t\tf(x0)\t\tf'(x0)\t\tx1\n");
    printf("----------------------------------------------------------\n");

    fx0 = postfixDegerlendir(postfix, postfixUzunluk, x0);
    double h = 0.0001;
    double fx0_plus_h =  postfixDegerlendir(postfix, postfixUzunluk, x0 + h);
    double  fx0_minus_h = postfixDegerlendir(postfix,  postfixUzunluk, x0 - h);
    dfx0 = (fx0_plus_h - fx0_minus_h) / (2 * h);

    x1 = (fabs(dfx0) > 1e-9) ? (x0 - fx0 / dfx0) : NAN;

    while (iterasyon < maksIterasyon && fabs(x1 - x0) >= hata && !isnan(x1)) {
        printf("%d\t%f\t%f\t%f\t%f\n", iterasyon + 1, x0, fx0, dfx0, x1);

        x0 = x1;
        iterasyon++;

        fx0 = postfixDegerlendir(postfix,  postfixUzunluk, x0);
        fx0_plus_h = postfixDegerlendir(postfix, postfixUzunluk, x0 + h);
        fx0_minus_h = postfixDegerlendir(postfix, postfixUzunluk, x0 - h);
        dfx0 = (fx0_plus_h - fx0_minus_h) / (2 * h);

        x1 = (fabs(dfx0) > 1e-9) ? (x0 - fx0 / dfx0) : NAN;
    }

    if (iterasyon  > 0 || (!isnan(x1) && fabs(x1 - x0) < hata )) {
         if (isnan(x1) && iterasyon > 0) {
             printf("%d\t%f\t%f\t%f\t(Türev sýfýr küçük)\n", iterasyon + 1, x0, fx0, dfx0);
         } else if (!isnan(x1)) {
             printf("%d\t%f\t%f\t%f\t%f\n", iterasyon + 1, x0, fx0, dfx0, x1);
         }
         }
    if (!isnan(x1) && (fabs(x1 - x0) < hata )) {
        printf("\nBulunan kok: %f\n", x1);
        converged = 1;
    } else if (isnan(x1) && iterasyon > 0) {
        printf("\nTürev sýfýra yaklaþtýðý için iþlem durduruldu. Son geçerli yaklaþým: %f\n", x0);
    } else if (iterasyon >= maksIterasyon && !converged) {
        printf(" Maksimum iterasyon sayisina ulasildi. Son yaklasim: %f\n", isnan(x1) ? x0 : x1);  }

	// Bellek temizle
    int i;
    for (i = 0; i < postfixUzunluk; i++) {
        free(postfix[i]);
        }
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
        }
    return x1;
}

void matrisTersiAl() {
    int n, i, j, k;
    printf("\nNxN'lik matrisin tersini alma islemi secildi.\n");
    printf("Matrisin boyutunu giriniz (N): ");
    scanf("%d", &n);

    // Dinamik bellek ayýrma
    double **matris = (double **)malloc(n * sizeof(double *));
    if (matris == NULL) {
        printf("Bellek ayirma hatasi!\n");
        return;
    }

    for (i = 0; i < n; i++) {
        matris[i] = (double *)malloc(2 * n * sizeof(double));
        if (matris[i] == NULL) {
            // Daha önce ayrýlan belleði temizle
            for (j = 0; j < i; j++) {
                free(matris[j]);
            }
            free(matris);
            printf("Bellek ayirma hatasi!\n");
            return;
        }
    }

    printf("%dx%d matrisin elemanlarini giriniz:\n", n, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("matris[%d][%d]: ", i + 1, j + 1);
            scanf("%lf", &matris[i][j]);
        }
    }

    printf("\nGirilen Matris:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%8.4f ", matris[i][j]);
        }
        printf("\n");
    }

    //Artýrýlmýþ matrisin sað tarafýný birim matris olarak ayarla
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j)
                matris[i][j + n] = 1.0;
            else
                matris[i][j + n] = 0.0;
        }
    }
    // Gauss-Jordan Eliminasyonu
    for (i = 0; i < n; i++) {
        // Pivot elemaný bul ve satýrý normalize et
        double pivot = matris[i][i];
        if (fabs(pivot) < 1e-9) { // Basit pivot kontrol
            printf("Matris tekil (singular), tersi alÄ±namaz.\n");
            // Bellek temizle
            for (j = 0; j < n; j++) {
                free(matris[j]);}
            free(matris);
            return;
        }
		for (j = i; j < 2 * n; j++) {
            matris[i][j] /= pivot;
        }
        // Diðer satýrlarý sýfýrla
        for (k = 0; k < n; k++) {
            if (k != i) {
                double carpan = matris[k][i];
                for (j = i; j < 2 * n; j++) {
                    matris[k][j] -= carpan * matris[i][j];
                }
                }
        }
        }
    printf("\nMatrisin Tersi:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%8.4f ", matris[i][j + n]);
        }
        printf("\n");}
    // Belleði temizle
    for (i = 0; i < n; i++) {
        free(matris[i]);
      }
    free(matris);
    }

void choleskyYontemi() {
    int n, i, j, k;
    printf("\nCholesky (ALU) yontemi ile Ax=b cozumu secildi.\n");
    printf("Matrisin boyutunu giriniz (N): ");
    scanf("%d", &n);

    double **A = (double **)malloc(n * sizeof(double *));
    double **L = (double **)malloc(n * sizeof(double *));
    double *b_vec = (double *)malloc(n * sizeof(double));
    double *y_vec = (double *)malloc(n * sizeof(double));
    double *x_vec = (double *)malloc(n * sizeof(double));

    if (A == NULL || L == NULL || b_vec == NULL || y_vec == NULL || x_vec == NULL) {
        printf("Bellek ayirma hatasi!\n");
        if (A) free(A);
        if (L) free(L);
        if (b_vec) free(b_vec);
        if (y_vec) free(y_vec);
        if (x_vec) free(x_vec);
        return;
    }
    for (i = 0; i < n; i++) {
        A[i] = (double *)malloc(n * sizeof(double));
        L[i] = (double *)calloc(n, sizeof(double));

        if (A[i] == NULL || L[i] == NULL) {
            printf("Matris satirlari icin bellek ayirma hatasi!\n");
            for (j = 0; j < i; j++) {
                if (A[j] != NULL) free(A[j]);
                if (L[j] != NULL) free(L[j]);
            }
            if (A[i] != NULL) free(A[i]);
            if (L[i] != NULL) free(L[i]);
            free(A);
            free(L);
            free(b_vec);
            free(y_vec);
            free(x_vec);
            return;
        }
        }
    printf("%dx%d simetrik pozitif tanimli matrisin (A) elemanlarini giriniz:\n", n, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("A[%d][%d]: ", i + 1, j + 1);
            scanf("%lf", &A[i][j]);
        }}

    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (fabs(A[i][j] - A[j][i]) > 1e-9) {
                printf("Hata: Girilen matris simetrik degil! Cholesky yontemi uygulanamaz.\n");
                for (k = 0; k < n; k++) {
                    free(A[k]);
                    free(L[k]);
                }
                free(A);
                free(L);
                free(b_vec);
                free(y_vec);
                free(x_vec);
                return;
            }
            }
             }
    printf("Ax=b denklem sistemi icin b vektorunun elemanlarini giriniz:\n");
    for (i = 0; i < n; i++) {
        printf("b[%d]: ", i + 1);
        scanf("%lf", &b_vec[i]);
    }

    printf("\nGirilen A Matrisi:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%8.4f ", A[i][j]);
        }
        printf("\n");
    }

    printf("\nGirilen b Vektoru:\n");
    for (i = 0; i < n; i++) {
        printf("%8.4f\n", b_vec[i]);
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j <= i; j++) {
            double sum = 0;
            if (j == i) {
                for (k = 0; k < j; k++) {
                    sum += L[j][k] * L[j][k];
                }
                double term_under_sqrt = A[j][j] - sum;
                if (term_under_sqrt <= 1e-9) {
                    printf("Hata: Matris pozitif tanimli degil (L[%d][%d] hesaplanamiyor, karekok ici: %f).\n", j,j, term_under_sqrt);
                    printf("Cholesky ayrýþtýrmasý yapýlamaz.\n");
                    for (k = 0; k < n; k++) {
                        free(A[k]);
                        free(L[k]);
                    }
                    free(A);
                    free(L);
                    free(b_vec);
                    free(y_vec);
                    free(x_vec);
                    return;
                }
                L[j][j] = sqrt(term_under_sqrt);
                } else {
                for (k = 0; k < j; k++) {
                    sum += L[i][k] * L[j][k];
                }
                if (fabs(L[j][j]) < 1e-9) {
                     printf("Hata: L matrisinde köþegen elemaný L[%d][%d] sýfýr, bölme hatasý olusacak.\n", j,j);
                     for (k = 0; k < n; k++) { free(A[k]); free(L[k]); }
                     free(A); free(L); free(b_vec); free(y_vec); free(x_vec);
                     return;
                }
                L[i][j] = (A[i][j] - sum) / L[j][j];
            }
            }}
    printf("\nL (Alt ucgensel) matrisi:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%8.4f ", L[i][j]);
        }
        printf("\n");
    }
    printf("\nL^T (Ust ucgensel) matrisi:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%8.4f ", L[j][i]);
        }
        printf("\n");}
 for (i = 0; i < n; i++) {
        double sum = 0;
        for (k = 0; k < i; k++) {
            sum += L[i][k] * y_vec[k];
        }
        if (fabs(L[i][i]) < 1e-9) {
             printf("Hata: L matrisinde köþegen elemaný L[%d][%d] sýfýr, ileri yerine koymada bölme hatasý.\n",i,i);
             for (k = 0; k < n; k++) { free(A[k]); free(L[k]); }
             free(A); free(L); free(b_vec); free(y_vec); free(x_vec);
             return;
        }
        y_vec[i] = (b_vec[i] - sum) / L[i][i];
        }
    for (i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (k = i + 1; k < n; k++) {
            sum += L[k][i] * x_vec[k];
        }
         if (fabs(L[i][i]) < 1e-9) {
             printf("Hata: L^T matrisinde (dolayýsýyla L'de) köþegen elemaný L[%d][%d] sýfýr, geri yerine koymada bölme hatasý.\n",i,i);
             for (k = 0; k < n; k++) { free(A[k]); free(L[k]); }
             free(A); free(L); free(b_vec); free(y_vec); free(x_vec);
             return;
        }
        x_vec[i] = (y_vec[i] - sum) / L[i][i];
    }

    printf("\nSonuc x (Ax=b):\n");
    for (i = 0; i < n; i++) {
        printf("x[%d] = %8.4f\n", i + 1, x_vec[i]);
    }
    for (i = 0; i < n; i++) {
        free(A[i]);
        free(L[i]);
    }
    free(A);
    free(L);
    free(b_vec);
    free(y_vec);
    free(x_vec);
}

void gaussSeidelYontemi() {
    int n, i, j, k, iterasyon = 0, maksIterasyon;
    double hata;
    printf("\nGauss-Seidel yontemi secildi.\n");
    printf("Denklem sisteminin boyutunu giriniz (N): ");
    scanf("%d", &n);

	// Dinamik bellek ayÄ±rma
	double **A = (double **)malloc(n * sizeof(double *));
    double *b = (double *)malloc(n * sizeof(double));
    double *x = (double *)calloc(n, sizeof(double)); // 0 ile baþlatýlýyor
    double *x_eski = (double *)malloc(n * sizeof(double));

    if (A == NULL || b == NULL || x == NULL || x_eski == NULL) {
        printf("Bellek  ayirma hatasi!\n");
        if (A != NULL) free(A);
        if (b != NULL) free(b);
        if (x != NULL) free(x);
        if (x_eski != NULL)  free(x_eski);
        return;
    }

    for (i = 0; i < n; i++) {
        A[i] = (double *)malloc(n * sizeof(double));
        if (A[i] == NULL) {
            printf("Bellek ayirma  hatasi!\n");
            // Daha önce ayrýlan belleði temizle
            for (j = 0; j < i; j++) {
                free(A[j]);
            }
            free(A);
            free(b);
            free(x);
            free(x_eski);
            return;
        }
    }

    // Katsayýlar matrisini al
    printf("Katsayilar  matrisini giriniz:\n");
    for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
            printf("A[%d][%d]: ", i + 1, j + 1);
            scanf("%lf", &A[i][j]);
        }
    }

    printf("\nGirilen Matris:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%8.4f ", A[i][j]);
        }
        printf("\n");
    }

    //Sabit terimler vektörünü al
    printf("Sabit terimler vektorunu giriniz:\n");
    for (i = 0; i < n; i++) {
        printf("b[%d]: ", i + 1);
        scanf("%lf", &b[i]);
    }

    printf("Hata toleransi: ");
    scanf("%lf", &hata);
    printf("Maksimum iterasyon sayisi: ");
    scanf("%d", &maksIterasyon);

    printf("\nIterasyon\t");
    for (i = 0; i < n; i++) {
        printf("x%d\t\t", i + 1);
    }
    printf("\n");

    // Gauss-Seidel iterasyonlarý
    do {
        // Önceki deðerleri sakla
        for (i = 0; i < n; i++) {
            x_eski[i] = x[i];
        }
        // Yeni deðerleri hesapla
        for (i = 0; i < n; i++) {
            double sum1 = 0, sum2 = 0;
            // Önceki deðiþkenler (zaten güncellenmiþ)
            for (j = 0; j < i; j++) {
                sum1 += A[i][j] * x[j];
            }
            // Sonraki deðiþkenler (henüz güncellenmemiþ)
            for (j = i + 1; j < n; j++) {
                sum2 += A[i][j] * x_eski[j];

			}
            // Yeni deðeri hesapla
            x[i] = (b[i] - sum1 - sum2) / A[i][i];
          }
        // Ýterasyon sonuçlarýný yazdýr
        printf("%d\t\t", iterasyon + 1);
        for (i = 0; i < n; i++) {
            printf("%f\t", x[i]);
         }
        printf("\n");
        // Yakýnsama kontrolü
        double maksHata = 0;
        for (i = 0; i < n; i++) {
            double gecerliHata = fabs(x[i] - x_eski[i]);
            if (gecerliHata > maksHata) {
                maksHata = gecerliHata;
            }
        }
        iterasyon++;
        if (maksHata < hata) {
            printf("\nCozum yakinsadi. Iterasyon sayisi: %d\n", iterasyon);
            break;
        }

    } while (iterasyon < maksIterasyon);
    if (iterasyon >= maksIterasyon) {
        printf("\nMaksimum iterasyon sayisina ulasildi. Cozum yakinsayamadi.\n");
    }
    // Son çözümü yazdýr
    printf("\nCozum vektoru:\n");
    for (i = 0; i < n; i++) {
        printf("x%d = %f\n", i + 1, x[i]);
    }

    //  Belleði temizle
    for (i = 0; i < n; i++) {
        free(A[i]);
      }
    free(A);
    free(b);
    free(x);
    free(x_eski);
}

void sayisalTurev(const char *denklem) {
    double x0, h, turev;
    int secim, i;

    printf("\nSayisal Turev hesaplama yontemi secildi.\n");
    printf("Hangi noktada turevi hesaplamak istiyorsunuz (x0): ");
    scanf("%lf", &x0);
    printf("Adim buyuklugu (h): ");
    scanf("%lf", &h);

    printf("\nTurev hesaplama yontemini seciniz:\n");
    printf("1. Ileri Fark\n");
    printf("2. Geri Fark\n");
    printf("3. Merkezi Fark\n");
    printf("Seciminiz: ");
    scanf("%d", &secim);

    // Denklemi tokenize et ve postfix'e çevir
    char *tokenler[256];
    int tokenSayisi = tokenizeDenklem(denklem, tokenler, 256);
    char *postfix[256];
    int postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);
    double fx0 = postfixDegerlendir(postfix, postfixUzunluk, x0);
    switch(secim) {
        case 1: // Ýleri Fark
            {
                double fx0_plus_h = postfixDegerlendir(postfix, postfixUzunluk, x0 + h);
                turev = (fx0_plus_h - fx0) / h;
                printf("\nIleri Fark Yontemi ile Turev: f'(%g) = %g\n", x0, turev);
            }
            break;
        case 2: // Geri Fark
            {
                 double fx0_minus_h = postfixDegerlendir(postfix, postfixUzunluk, x0 - h);
                 turev = (fx0 - fx0_minus_h) / h;
                 printf("\nGeri Fark Yontemi ile Turev: f'(%g) = %g\n", x0, turev);
            }
            break;
        case 3: // Merkezi Fark
            {
                double fx0_plus_h = postfixDegerlendir(postfix, postfixUzunluk, x0 + h);
                double fx0_minus_h = postfixDegerlendir(postfix, postfixUzunluk, x0 - h);
                turev = (fx0_plus_h - fx0_minus_h) / (2 * h);
                printf("\nMerkezi Fark Yontemi ile Turev: f'(%g) = %g\n", x0, turev);
            }
            break;
        default:
            printf("Gecersiz secim!\n");
            break;
    }
    // Bellek temizlik
    for (i = 0; i < postfixUzunluk; i++) {
        free(postfix[i]);
    }
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
        }
        }

void simpsonYontemi(const char *denklem) {
    double a, b, integral = 0.0;
    int n, secim, i;

    printf("\nSimpson yontemi secildi.\n");
    printf("Integral alt siniri (a): ");
    scanf("%lf", &a);
    printf("Integral ust siniri (b): ");
    scanf("%lf", &b);

    printf("\nSimpson kuralini seciniz:\n");
    printf("1. Simpson 1/3 Kurali\n");
    printf("2. Simpson 3/8 Kurali\n");
    printf("Seciminiz: ");
    scanf("%d", &secim);

    // Denklemi tokenize et ve postfix'e çevirr
    char *tokenler[256];
    int tokenSayisi = tokenizeDenklem(denklem, tokenler, 256);
    char *postfix[256];
    int postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);

    if (secim == 1) { // Simpson 1/3 Kural
        printf("Bolme sayisi (cift sayi olmali): ");
        scanf("%d", &n);
        if (n % 2 != 0) {
            printf("Bolme sayisi cift olmali! Otomatik olarak %d yapildi.\n", n+1);
            n++;
        }
        double h = (b - a) / n;
        double x, fx;
        // Ýlk ve son deðerler
        double fa = postfixDegerlendir(postfix, postfixUzunluk, a);
        double fb = postfixDegerlendir(postfix, postfixUzunluk, b);
        integral = fa + fb;

        // Çift indeksli terimler (2 ile çarpýlacak)
        for (i = 2; i < n; i += 2) {
            x = a + i * h;
            fx = postfixDegerlendir(postfix, postfixUzunluk, x);
            integral += 2 * fx;
        }

        // Tek indeksli terimler (4 ile çarpýlacak)
        for (i = 1; i < n; i += 2) {
            x = a + i * h;
            fx = postfixDegerlendir(postfix, postfixUzunluk, x);
            integral += 4 * fx;
        }
        integral = integral * h / 3.0;
        printf("\nSimpson 1/3 Kurali ile Integral: %g\n", integral);
    }
    else if (secim == 2) { // Simpson 3/8 Kuralý
        printf("Bolme sayisi (3'un kati olmali): ");
        scanf("%d", &n);
        if (n % 3 != 0) {
            printf("Bolme sayisi 3'un kati olmali! Otomatik olarak %d yapildi.\n", n + (3 - n % 3));
            n += (3 - n % 3);
        }
        double h = (b - a) / n;
        double x, fx;
        double fa = postfixDegerlendir(postfix, postfixUzunluk, a);
        double fb = postfixDegerlendir(postfix, postfixUzunluk, b);
        integral = fa + fb;
        // 3'ün katý olan indeksli terimler (2 ile çarpýlacak)
        for (i = 3; i < n; i += 3) {
            x = a + i * h;
            fx = postfixDegerlendir(postfix, postfixUzunluk, x);
            integral += 2 * fx;
        }
        // Diðer terimler (3 ile çarpýlacak)
        for (i = 1; i < n; i++) {
            if (i % 3 != 0) {
                x = a + i * h;
                fx = postfixDegerlendir(postfix, postfixUzunluk, x);
                integral += 3 * fx;
            }
            }
        integral = integral * 3 * h / 8.0;
        printf("\nSimpson 3/8 Kurali ile Integral: %g\n", integral);
    }
    else {
        printf("Gecersiz secim!\n");
    }
    // Bellek temizliði
    for (i = 0; i < postfixUzunluk; i++) {
        free(postfix[i]);
    }
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
    }
}

void trapezYontemi(const char *denklem) {
    double a, b, integral = 0.0;
    int n, i;

    printf("\nTrapez yontemi secildi.\n");
    printf("Integral alt siniri (a): ");
    scanf("%lf", &a);
    printf("Integral ust siniri (b): ");
    scanf("%lf", &b);
    printf("Bolme sayisi (n): ");
    scanf("%d", &n);

    //Denklemi tokenize et ve postfix'e çevir
    char *tokenler[256];
    int tokenSayisi = tokenizeDenklem(denklem, tokenler, 256);
    char *postfix[256];
    int postfixUzunluk = shuntingYard(tokenler, tokenSayisi, postfix);

    double h = (b - a) / n;
    double x, fx;

    // Ýlk ve son deðerler (1 kez hesaplanacak)
    double fa = postfixDegerlendir(postfix, postfixUzunluk, a);
    double fb = postfixDegerlendir(postfix, postfixUzunluk, b);
    integral = fa + fb;

    // Ara deðerler (2 kez hesaplanacak)
    for (i = 1; i < n; i++) {
        x = a + i * h;
        fx = postfixDegerlendir(postfix, postfixUzunluk, x);
        integral += 2 * fx;
    }

    integral = integral * h / 2.0;
    printf("\nTrapez Yontemi ile Integral: %g\n", integral);

    // Bellek temizliÄði
    for (i = 0; i < postfixUzunluk; i++) {
        free(postfix[i]);
    }
    for (i = 0; i < tokenSayisi; i++) {
        if (tokenler[i] != NULL) {
            free(tokenler[i]);
            tokenler[i] = NULL;
        }
    }
}

void gregoryNewtonEnterpolasyonu() {
    int n, i, j;
    printf("\nGregory-Newton enterpolasyonu secildi.\n");
    printf("Nokta sayisini giriniz: ");
    scanf("%d", &n);
    //Dinamik bellek ayýrma
    double *x = (double *)malloc(n * sizeof(double));
    double *y = (double *)malloc(n * sizeof(double));
    double **fark = (double **)malloc(n * sizeof(double *));
    if (x == NULL || y == NULL || fark == NULL) {
        printf("Bellek ayirma hatasi!\n");
        if (x != NULL) free(x);
        if (y != NULL) free(y);
        if (fark != NULL) free(fark);
        return;
    }
    for (i = 0; i < n; i++) {
        fark[i] = (double *)malloc(n * sizeof(double));
        if (fark[i] == NULL) {
            printf("Bellek ayirma hatasi!\n");
            for (j = 0; j < i; j++) {
                free(fark[j]);
            }
            free(fark);
            free(x);
            free(y);
            return;
        }
    }

    // Noktalarý al
    printf("Noktalari giriniz:\n");
    for (i = 0; i < n; i++) {
        printf("x[%d]: ", i);
        scanf("%lf", &x[i]);
        printf("y[%d]: ", i);
        scanf("%lf", &y[i]);
    }

    // Ýleri farklarý hesapla
    for (i = 0; i < n; i++) {
        fark[i][0] = y[i];
    }
    for (j = 1; j < n; j++) {
        for (i = 0; i < n - j; i++) {
            fark[i][j] = fark[i+1][j-1] - fark[i][j-1];
        }
    }
    // Enterpolasyon için x deðeri al
    double x_deger;
    printf("Enterpolasyon icin x degerini giriniz: ");
    scanf("%lf", &x_deger);
    // Gregory-Newton enterpolasyonu hesapla
    double h = x[1] - x[0]; // Eþit aralýklý x deðerleri için
    double u = (x_deger - x[0]) / h;
    double sonuc = fark[0][0];
    double u_carpim = 1.0;

    for (i = 1; i < n; i++) {
        u_carpim *= (u - i + 1) / i;
        sonuc += u_carpim * fark[0][i];
    }
    printf("\nGregory-Newton enterpolasyonu ile f(%g) = %g\n", x_deger, sonuc);
    // Bellekk temizle
    for (i = 0; i < n; i++) {
        free(fark[i]);
    }
    free(fark);
    free(x);
    free(y);
}


int main() {
    int secim;
    char denklem[256];
    do {
        printf("\n===== SAYISAL ANALIZ YONTEMLERI =====\n");
        printf("1. Bisection yontemi\n");
        printf("2. Regula-Falsi yontemi\n");
        printf("3. Newton-Rapshon yontemi\n");
        printf("4. NxN'lik bir matrisin tersi\n");
        printf("5. Cholesky (ALU) yontemi\n");
        printf("6. Gauss-Seidel yontemi\n");
        printf("7. Sayisal Turev\n");
        printf("8. Simpson yontemi\n");
        printf("9. Trapez yontemi\n");
        printf("10. Degisken donusumsuz Gregory-Newton enterpolasyonu\n");
        printf("0. Cikis\n");
        printf("Seciminizi yapiniz: ");
        scanf("%d", &secim);
        switch(secim) {
            case 0:
                printf("Programdan cikiliyor...\n");
                break;
            case 1:
                printf("Bisection yontemi secildi.\n");
                denklemAl(denklem, 256);
                printf("Girilen denklem: %s\n", denklem);
                denklemYazdir(denklem);
                bisectionYontemi(denklem);
                break;
            case 2:
                printf("Regula-Falsi yontemi secildi.\n");
                denklemAl(denklem, 256);
                printf("Girilen denklem: %s\n", denklem);
                denklemYazdir(denklem);
                regulaFalsiYontemi(denklem);
                break;
            case 3:
                printf("Newton-Rapshon yontemi secildi.\n");
                denklemAl(denklem, 256);
                printf("Girilen denklem: %s\n", denklem);
                denklemYazdir(denklem);
                newtonRaphsonYontemi(denklem);
                break;
            case 4:
                matrisTersiAl();
                break;
            case 5:
                printf("Cholesky (ALU) yontemi secildi.\n");
                choleskyYontemi();
                break;
            case 6:
                printf("Gauss-Seidel yontemi secildi.\n");
                gaussSeidelYontemi();
                break;
            case 7:
                printf("Sayisal Turev secildi.\n");
                denklemAl(denklem, 256);
                printf("Girilen denklem: %s\n", denklem);
                denklemYazdir(denklem);
                sayisalTurev(denklem);
                break;
            case 8:
                printf("Simpson yontemi secildi.\n");
                denklemAl(denklem, 256);
                printf("Girilen denklem: %s\n", denklem);
                denklemYazdir(denklem);
                simpsonYontemi(denklem);
                break;
            case 9:
                printf("Trapez yontemi secildi.\n");
                denklemAl(denklem, 256);
                printf("Girilen denklem: %s\n", denklem);
                denklemYazdir(denklem);
                trapezYontemi(denklem);
                break;
            case 10:
                printf("Degisken donusumsuz Gregory-Newton enterpolasyonu secildi.\n");
                gregoryNewtonEnterpolasyonu();
                break;
            default:
                printf("Gecersiz secim! Lutfen tekrar deneyin.\n");
        }
    } while(secim != 0);

    return 0;
}
