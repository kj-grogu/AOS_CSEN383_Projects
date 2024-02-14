import java.util.*;
import java.text.DecimalFormat;

class Main {

  public static class Result {
    public double hitmissRatio;
    public int swappedInProcesses;
  }

  public static int NUMBER_OF_PROCESSES = 150;
  public static int SIMULATION_DURATION = 60;
  public static int MAX_PROCESSES_DURATION = 5;
  public static int PAGE_LIST_SIZE = 100;
  public static int hitMissRatioPerRun;
  private static int page_count_options[] = { 5, 11, 17, 31 };
  private static String[] Algorithms = new String[] { "FIFO", "LRU", "LFU", "MFU", "Random" };
  private static final int ALGO_COUNT = Algorithms.length;
  private static final int ITERATIONS = 5;
  private static final Random rand = new Random();

  public static void main(String[] args) {
    Result[][] results = new Result[ALGO_COUNT][ITERATIONS];
    for (int i = 0; i < ITERATIONS; i++) {
      Process processes[] = generateProcesses();
      for (int algo = 0; algo < ALGO_COUNT; algo++) {
        Process[] copy = cloneProces(processes);
        System.out
            .println("\n######### Executiing '" + Algorithms[algo] + "' algorithm for " + i + " iteration. #########");
        results[algo][i] = executeAlgorithm(algo, copy);
      }
    }

    for (int i = 0; i < ITERATIONS; i++) {
      System.out.println("\nAlgo Type: " + Algorithms[i]);
      double hitRatioSum = 0;
      int swappedInProcessesSum = 0;
      for (int j = 0; j < ITERATIONS; j++) {
        hitRatioSum += results[i][j].hitmissRatio;
        swappedInProcessesSum += results[i][j].swappedInProcesses;
      }

      System.out.println("Hit Ratio average: " + hitRatioSum / ITERATIONS);
      System.out.println("Average Swapped in Processes: " + swappedInProcessesSum / ITERATIONS);
    }
  }

  static Result executeAlgorithm(int algoType, Process[] processes) {
    int time;
    int swapped = 0;
    int hit = 0, miss = 0;
    hitMissRatioPerRun = 0;

    Page currentPage = new Page();
    Page page = new Page();
    ArrayList<Page> pages = page.getPageList();

    int processIndex = 0;
    double referencedTime = 0.0;
    DecimalFormat df = new DecimalFormat("#.0");

    for (time = 0; time < SIMULATION_DURATION; time++) {
      int numberOfStartedProcesses = initiateProcesses(processIndex, processes, time, page, pages);
      swapped += numberOfStartedProcesses;
      processIndex += numberOfStartedProcesses;

      for (int k = 1; k < 10; k++) {
        for (int j = 0; j < processIndex; j++) {
          if (processes[j].serviceDuration <= 0)
            continue;

          processes[j].currentPage = page.getNextPageNumber(processes[j].currentPage, processes[j].processSizeInPages);

          if (page.existsInMemory(pages, processes[j].processId, processes[j].currentPage)) {
            currentPage = page.getPage(pages, processes[j].processId, processes[j].currentPage);
            currentPage.lastReferenced = time;
            currentPage.count++;
            hit++;
            pages = page.updatePageList(currentPage, pages);
            referencedTime = time + (0.1 * k);
            System.out.println("Time " + (df.format(referencedTime)) + ": Page " + processes[j].currentPage
                + " for process " + processes[j].processId + " already exists in memory at location " + currentPage.index);
          } else {
       boolean freePageAvail = true;
       Page availablePage = page.getFreePage(pages);
            if (availablePage == null) {
              miss++;
        freePageAvail = false;
        switch (algoType) {
                case 0:
                  pages = FirstComeFirstServe.execute(pages);
                  break;
                case 1:
                  pages = LeastRecentlyUsed.execute(pages);
                  break;
                case 2:
                  pages = LeastFrequentlyUsed.execute(pages);
                  break;
                case 3:
                  pages = MostFrequentlyUsed.execute(pages);
                  break;
                case 4:
                  pages = RandomPick.execute(pages);
                  break;
              }

              availablePage = page.getFreePage(pages);
            }

            availablePage.processId = processes[j].processId;
            availablePage.pageNo = processes[j].currentPage;
            availablePage.broughtTime = referencedTime;
            availablePage.lastReferenced = referencedTime;
            availablePage.count = 0;
            pages = page.updatePageList(availablePage, pages);

            System.out.println("Time " + (df.format(referencedTime)) + ": Page " + processes[j].currentPage
                + " for Process " + processes[j].processId + ", brought into memory page number " +  ". Eviction needed: " + !freePageAvail);
          }
        }
      }

      for (int a = 0; a < processIndex; a++) {
        if (processes[a].serviceDuration > 0) {
          processes[a].serviceDuration--;
          if (processes[a].serviceDuration == 0) {
            page.printMemoryMap(pages);
            page.freeMemory(pages, processes[a].processId);
            System.out.println("Finished the process " + processes[a].processId + " Time: " + time + " Size: "
                + processes[a].processSizeInPages + " duration: " + processes[a].serviceDuration);
          }
        }
      }
    }

    Result result = new Result();
    result.hitmissRatio = miss == 0 ? hit : hit * 1.0 / miss;
    result.swappedInProcesses = swapped;
    return result;
  }

  static Process[] generateProcesses() {
    Process[] processes = new Process[NUMBER_OF_PROCESSES];

    for (int j = 0; j < NUMBER_OF_PROCESSES; j++) {
      rand.setSeed(j * 10000000);
      processes[j] = new Process(j, page_count_options[rand.nextInt(4)], rand.nextInt(60),
          1 + rand.nextInt(MAX_PROCESSES_DURATION), 0);
    }

    Arrays.sort(processes, new Comparator<Process>() {
      @Override
      public int compare(Process p1, Process p2) {
        return p1.arrivalTime - p2.arrivalTime;
      }
    });

    return processes;
  }

  static Process[] cloneProces(Process[] originalProcesses) {
    Process[] clonedProcesses = new Process[NUMBER_OF_PROCESSES];
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
      Process originalProcess = originalProcesses[i];
      clonedProcesses[i] = new Process(originalProcess.processId, originalProcess.processSizeInPages,
          originalProcess.arrivalTime, originalProcess.serviceDuration,
          originalProcess.currentPage);
    }
    return clonedProcesses;
  }

  static int initiateProcesses(int processQueueIndex, Process[] processes, int time, Page pageObj,
      ArrayList<Page> pages) {
    int initiatedProcessCount = 0;
    while (processQueueIndex < NUMBER_OF_PROCESSES && processes[processQueueIndex].arrivalTime <= time) {
      if (!pageObj.checkFreePages(pages, 4))
        break;
      Page page = pageObj.getFreePage(pages);
      page.processId = processes[processQueueIndex].processId;
      page.pageNo = processes[processQueueIndex].currentPage;
      page.broughtTime = 1.0 * time;
      page.count = 1;
      page.lastReferenced = time;
      pages = pageObj.updatePageList(page, pages);
      System.out.println("\nStarting the process " + page.processId + " Time: " + time + " Size: "
          + processes[processQueueIndex].processSizeInPages + " duration: "
          + processes[processQueueIndex].serviceDuration);
      pageObj.printMemoryMap(pages);
      initiatedProcessCount++;
      processQueueIndex++;
    }

    return initiatedProcessCount;
  }
}