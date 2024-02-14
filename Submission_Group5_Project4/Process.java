class Process{
  public int processId;
  public int processSizeInPages;
  public int arrivalTime;
  public int serviceDuration;
  public int currentPage;

  public Process(int pid, int page_count,int arrival_time,int duration,int curr_page){
    this.processId = pid;
    this.processSizeInPages = page_count;
    this.arrivalTime = arrival_time;
    this.serviceDuration = duration;
    this.currentPage = curr_page;
  }
}
