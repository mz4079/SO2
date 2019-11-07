#include "common.h"
int main(int argc, char **argv)
{
    sem_t* sem = sem_open("/msg_signal", 0);
    err(sem == SEM_FAILED, "sem_open");
    
    sem_t* sem2 = sem_open("/msg_signal2", 0);
    err(sem2 == SEM_FAILED, "sem_open");
    
    int fd = shm_open("/msg_data", O_RDWR, 0600);
    err(fd == -1, "shm_open");
    
    struct data_t* pdata = (struct data_t*)mmap(NULL, sizeof(struct data_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    err(pdata == NULL, "mmap");
    
    while(1) {
        printf("Podaj dzialanie: ");
        float num1, num2;
        char op;
        scanf("%f%c%f",&num1,&op,&num2);
        sem_wait(&pdata->cs);
        pdata->num1 = num1;
        pdata->num2 = num2;
        pdata->op = op;
        sem_post(&pdata->cs);
		sem_post(sem);
		sem_wait(sem2);
        printf("%f\n",pdata->result);
 
    }
    
    sem_close(sem);
    sem_close(sem2);
    munmap(pdata, sizeof(struct data_t));
    close(fd);
	return 0;
}
