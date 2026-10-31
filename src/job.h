#define MAX_JOBS 100
typedef enum{
    JOB_WAITING,
    JOB_RUNNING,
    JOB_COMPLETED
} job_status_t;

typedef struct{
    int process_id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    job_status_t status;
} job_t;

int add_job(job_t jobs[], int *count);
void display_jobs(job_t jobs[], int count);
void initialize_job(job_t *job);
