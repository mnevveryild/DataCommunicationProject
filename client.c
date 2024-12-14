
#include <stdio.h> // giriş/çıkış işlemleri için kullanılır.
#include <stdlib.h> // bellek yönetimi ve diğer yardımcı işlemler için kullanılır.
#include <string.h> // string manipülasyonu için kullanılır.
#include <unistd.h> // UNIX sistem çağrıları için kullanılır
#include <arpa/inet.h> // ağ işlemleri ve soket programlama için kullanılır.

#define PORT 8080 // sunucu ile iletişim için kullanılacak port numarası.
#define BUFFER_SIZE 1024 // veri gönderme ve alma için buffer boyutu.
#define QUESTIONS_TO_ASK 4 
 
int main() { 
    int sock; // istemci soket tanımı.
    struct sockaddr_in server_address; // sunucu adres bilgilerini tutan yapı.
    char buffer[BUFFER_SIZE];  // veri alışverişi için buffer.
 
    // soket oluşturuluyor.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("Socket creation error");  // hata mesajı yazdırılıyor.
        return -1; 
    } 
 
    server_address.sin_family = AF_INET; // IPv4 adres ailesi kullanılacak.
    server_address.sin_port = htons(PORT); // port numarası ağ byte sırasına çevriliyor.
 
 // IP adresi metinden binary formata dönüştürülüyor.
     if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) { 
        perror("Invalid address/ Address not supported"); // hata mesajı yazdırılıyor.
        return -1; 
    } 
 
    // sunucuya bağlanılıyor.
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { 
        perror("Connection failed"); // bağlantı hatası mesajı.
        return -1; 
    } 
 
    printf("Connected to the server!\n\n"); // bağlantı başarılı mesajı
 
     // sunucudan sorular alınıyor ve cevaplar gönderiliyor.
    for (int i = 0; i < QUESTIONS_TO_ASK; i++) { // 4 soru için döngü.
        memset(buffer, 0, BUFFER_SIZE);  // buffer temizleniyor
        int valread = read(sock, buffer, BUFFER_SIZE); // sunucudan veri okunuyor.
 
        if (valread <= 0) { // bağlantı kesilirse döngüden çık.
            printf("Server closed the connection.\n"); 
            break; 
        } 
 
        printf("%s\n", buffer);  // gelen soru ekrana yazdırılıyor.
 
        printf("Enter your answer: "); 
        fgets(buffer, BUFFER_SIZE, stdin); // kullanıcıdan cevap alınıyor.
 
         // kullanıcı cevabını sunucuya gönder.
        send(sock, buffer, strlen(buffer), 0); 
    } 
 
    // final skoru al.
    memset(buffer, 0, BUFFER_SIZE); // buffer temizleniyor
    read(sock, buffer, BUFFER_SIZE);  // final skoru okunuyor.
    printf("%s\n", buffer); // final skoru ekrana yazdırılıyor.
 
    close(sock); // soket kapatılıyor.
    return 0; 
}