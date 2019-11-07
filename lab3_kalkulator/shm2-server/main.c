#include "../shm2-client/common.h"

int main(int argc, char **argv)
{
	sem_t* sem = sem_open("/msg_signal", O_CREAT, 0600, 0);
    err(sem == SEM_FAILED, "sem_open");
	
	sem_t* sem2 = sem_open("/msg_signal2", O_CREAT, 0700, 0);
    err(sem2 == SEM_FAILED, "sem_open");
    
    int fd = shm_open("/msg_data", O_CREAT | O_RDWR, 0600);
    err(fd == -1, "shm_open");
    
    ftruncate(fd, sizeof(struct data_t));
    struct data_t* pdata = (struct data_t*)mmap(NULL, sizeof(struct data_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    err(pdata == NULL, "mmap");
    
    sem_init(&pdata->cs, 1, 1);
    printf("Gotowe, czekam na klienta\n");
    
    while (1) {
        sem_wait(sem);
        sem_wait(&pdata->cs);
        pdata->result = pdata->num1 + pdata->num2;
        sem_post(&pdata->cs);
		sem_post(sem2);
    }
    munmap(pdata, sizeof(struct data_t));
    close(fd);
    shm_unlink("/msg_data");
    sem_close(sem);
    return 0;
}
