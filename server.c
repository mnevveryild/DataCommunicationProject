#include <stdio.h> // giriş/çıkış işlemleri için kullanılır.
#include <stdlib.h> // bellek yönetimi ve rastgele sayı üretimi gibi işlemler için kullanılır.
#include <string.h> // string manipülasyonu için kullanılır.
#include <unistd.h> // UNIX sistem çağrıları için kullanılır 
#include <arpa/inet.h> // ağ işlemleri ve soket programlama için kullanılır.
#include <pthread.h> // çoklu iş parçacığı işlemleri için kullanılır.


#define PORT 8080 // sunucunun dinleyeceği port numarası.
#define MAX_CLIENTS 5 // maksimum istemci sayısı.
#define BUFFER_SIZE 1024 // veri aktarımı için buffer boyutu.
#define QUESTIONS_TO_ASK 4 // her istemciye sorulacak toplam soru sayısı

// sorular ve cevaplar 
const char *questions[] = { 
    "In which communication,the signal havingshortest wavelengt is used?\nA) Satellite communication\nB) Communication with UTP cable\nC) Communication with coaxial cable\nD) Phone communication\nE) Communication with optical fiber", 
    "Which one is wrong for packet switching?\nA) Each message is sent in packets\nB) A systemcan communicate more than one system at the same time with packet switching\nC) It is ideal for phone communication\nD) ATM based on packet switching technology\nE) None", 
    "Which property of a signal can be seen in the frequency domain?\nA) Phase\nB) Period\nC) Wavelength\nD) Spectrum\nE)All", 
    "Which definition is not true for OSI Architecture? \nA)Service ports are used in the communication between layers in the same system. \nB) For the communication between networks, Internet layer is used. \nC) Same layers in different systems communicate with protocols. \nD) Communication between upper and lower layers is done with services. \nE) Confirmed or unconfirmed communication can be done. ", 
    "Which is not true for digital signal transmission? \nA) It is done with discrete signal parts. \nB) It is used in LAN. \nC) Data rate unit is defined as bps. \nD) It gets more attenuation in transmission. \nE) It must be amplified after a certain distance.", 
    "Which one is not true for TCP/IP protocol architecture? \nA) Port address is used for identifying each computer in the network uniquely. \nB) TCP is the most reliable transport layer protocol. \nC)Isolating above layers from network technology is done with Internet layer. \nD) FTP is one of the application layer protocols. \nE) Internet is based on TCP/IP protocols. ", 
    "How does delay occur in signal transmission? \nA)Not reaching the receiver due to weakness of the signal. \nB) Signal reaches the receiver late due to interference of another signal. \nC) Because of the thermal noise, signal transmission gets slowed down. \nD) Frequencies at edges arrive to the receiver later in cable transmission. \nE) All" ,
    "Which one is done by Repeater?\nA) Providing transmission of analog signal\nB) Correcting the distortion on the digital signal\nC) Converting analog signal into digital signal\nD) Preventing delay in the transmission\nE) Reducing thermal noise "
}; 
const char answers[] = {'E', 'C', 'D', 'B', 'E', 'A', 'D', 'B'}; 
// istemci bilgilerini tutan yapı.
typedef struct { 
    int sock; // istemcinin soket numarası.
    int score; // istemcinin skoru.
} ClientData; 
 
ClientData clients[MAX_CLIENTS];   // maksimum istemci için yapı dizisi.
int client_count = 0;  // şu anda bağlı olan istemci sayısı.
 
 //istemcilerle iletişim kurmak için iş parçacığı işlevi.
void *handle_client(void *client_socket) { 
    int sock = *(int *)client_socket; // istemci soketi alınıyor.
    free(client_socket); // dinamik bellek serbest bırakılıyor.
 
    char buffer[BUFFER_SIZE]; // veri alışverişi için buffer tanımlanıyor.
    int question_count = sizeof(questions) / sizeof(questions[0]); // toplam soru sayısı hesaplanıyor.
    int asked_questions[QUESTIONS_TO_ASK]; // rastgele seçilecek sorular için diz.
    int score = 0; // istemcinin skoru başlangıçta sıfır.

 
     // rastgele sorular seçiliyor 
    srand(time(NULL) + sock); 
    for (int i = 0; i < QUESTIONS_TO_ASK; i++) { 
        int random_index; 
        int duplicate; 
        do { 
            duplicate = 0; 
            random_index = rand() % question_count; 
            for (int j = 0; j < i; j++) { 
                if (asked_questions[j] == random_index) { // sorular arasından rastgele bir indeks seçiliyor.
                    duplicate = 1; // daha önce seçilmişse tekrar dene.
                    break; 
                } 
            } 
        } while (duplicate); 
        asked_questions[i] = random_index; // benzersiz indeks kaydediliyor.
    } 
 
    for (int i = 0; i < QUESTIONS_TO_ASK; i++) { 
        int question_index = asked_questions[i];  // sorunun indeksi alınıyor.
 
        // soruyu istemciye gönder.
          send(sock, questions[question_index], strlen(questions[question_index]), 0); 
 
   // istemciden cevap al.
          memset(buffer, 0, BUFFER_SIZE); 
        int valread = recv(sock, buffer, BUFFER_SIZE, 0); 
        if (valread <= 0) { 
            printf("Client disconnected prematurely.\n"); 
            close(sock); 
            return NULL; 
        } 
 
         // cevabı kontrol et ve geri bildirim gönder.
        if (buffer[0] == answers[question_index]) { 
            score++; 
            send(sock, "Correct\n", 8, 0); 
        } else { 
            send(sock, "Wrong\n", 6, 0); 
        } 
    } 
 
     // final skorunu istemciye gönder.
    snprintf(buffer, BUFFER_SIZE, "Your final score is: %d/%d\n", score, QUESTIONS_TO_ASK); 
    send(sock, buffer, strlen(buffer), 0); 
 
    close(sock); // soket kapatılıyor.
    return NULL; 
} 
 
int main() { 
    int server_fd, new_socket; // sunucu ve yeni istemci soketi tanımları.
    struct sockaddr_in address; // sunucunun adres bilgileri.
    int addrlen = sizeof(address); // adres boyutu.

 
 // sunucu soketi oluşturuluyor.
     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("Socket failed"); // hata mesajı yazdırılıyor.
        exit(EXIT_FAILURE); 
    } 
 
    // adres ve port bilgileri ayarlanıyor.
    address.sin_family = AF_INET; // IPv4 kullanılacak.
    address.sin_addr.s_addr = INADDR_ANY; // tüm arayüzlerden bağlantı kabul et.
    address.sin_port = htons(PORT); // port numarası ağ byte sırasına çevriliyor.

  // soket porta bağlanıyor.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { 
        perror("Bind failed"); // hata mesajı yazdırılıyor.
        close(server_fd); 
        exit(EXIT_FAILURE); 
    } 
 
// bağlantılar dinleniyor.
    if (listen(server_fd, MAX_CLIENTS) < 0) { 
        perror("Listen failed"); 
        close(server_fd); 
        exit(EXIT_FAILURE); 
    } 
 
    printf("Server is running on port %d\n", PORT); 
 
// istemciler kabul ediliyor.
    while (client_count < MAX_CLIENTS) { 
        int *client_socket = malloc(sizeof(int));  // yeni istemci soketi için bellek ayrılıyor.
        if ((*client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed"); // hata mesajı yazdırılıyor.
            free(client_socket); 
            continue; 
        } 
 
        printf("New client connected!\n"); 
 
         // istemci bilgileri kaydediliyor.
        clients[client_count].sock = *client_socket; 
        clients[client_count].score = 0; 
        client_count++; 
          // istemci için yeni bir iş parçacığı başlatılıyor.
        pthread_t thread_id; 
        pthread_create(&thread_id, NULL, handle_client, client_socket); 
        pthread_detach(thread_id); // iş parçacığı otomatik temizleme moduna alınıyor.
    }
    } 
 
    close(server_fd); // sunucu soketi kapatılıyor.
    return 0; 
