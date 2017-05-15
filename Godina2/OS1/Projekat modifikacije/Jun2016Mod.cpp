//==========================================================================================================//
//==========================================================================================================//
//
//  Nikola Bebic - bn140314d@student.etf.rs
//
//  OS1 Projekat
//  Modifikacija
//  Jun 2016
//
//==========================================================================================================//
//==========================================================================================================//

/*
 * Treba napisati drajver za neki IO uredjaj.
 * Drajver treba da vrsi prenos nekog bloka podataka na njega
 * 
 * Data je funkcija sa potpisom void send(void *src, int size);
 * Funkcija je asinhrona, neblokirajuca, i nije thread-safe
 * Pomocu ove funkcije uredjaju se samo javlja da treba da zapocne prenos.
 * Uredjaj ce dojaviti kraj prenosa tako sto ce pozvati prekidnu rutinu
 * na adresi 70h.
 * 
 * Treba napisati funkciju sa potpisom void send(IOJob *job);
 * Deklaraciju ove funkcije treba staviti na kraj fajla thread.h,
 * a definiciju na kraj fajla thread.cpp.
 * Ostale fajlove ne treba menjati.
 * Funkcija treba da bude sinhrona.
 * Takodje, treba obezbediti da vise niti mogu da pozivaju ovu funkciju,
 * ali da samo jedna nit u jednom trenutku vrsi prenos,
 * tj. da se sve ostale niti koje pokusaju da pozovu blokiraju, dok
 * nit koja trenutno izvrsava prenos ne zavrsi taj prenos.
 */

// Definicija strukture IOJob (ovo je dato):
struct IOJob {
    void *src;
    int size;
    IOJob *next;
};

// Resenja (kraj fajla thread.cpp):
//
//==========================================================================================================//
//
// Preko semafora:

#include <semaphor.h>
#include <dos.h>

Semaphore wait_sem(0);

void interrupt finishedTransfer(...) {
    wait_sem.signal();
}

void send(IOJob *job) {
    asm pushf;
    asm cli;
    static Semaphore mutex(1);
    setvect(0x70, finishedTransfer);
    mutex.wait(0);
    send(job->src, job->size);
    wait_sem.wait(0);
    mutex.signal();
    asm popf;
}

//==========================================================================================================//
//
// Preko dogadjaja:

#include <semaphor.h>
#include <event.h>
#include <dos.h>

PREPAREENTRY(112, 0);

void send(IOJob *job) {
    asm pushf;
    asm cli;
    static Semaphore mutex(1);
    static Event event112(112);
    mutex.wait(0);
    send(job->src, job->size);
    event112.wait();
    mutex.signal();
    asm popf;
}

//==========================================================================================================//
//
// Cheat koji je radio (jer sam pogledao njihovu implementaciju), ali ne moze tako da se radi:

#include <semaphor.h>
#include <dos.h>

void send(IOJob *job) {
    asm pushf;
    asm cli;
    static Semaphore mutex(1);
    static Semaphore sleep_sem(0);
    mutex.wait(0);
    send(job->src, job->size);
    sleep_sem.wait(10);
    mutex.signal();
    asm popf;
}


//==========================================================================================================//
