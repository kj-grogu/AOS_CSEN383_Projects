import java.util.*;

class RandomPick {
  public static ArrayList<Page> execute(ArrayList<Page> pageArrayList) {
    Random rand = new Random();
    int random = rand.nextInt(Main.PAGE_LIST_SIZE);
    Page page = new Page();
    int count = 0;
    for (Page p : pageArrayList) {
      if (p.processId > 0 && count < random)
        page = p;

      count++;
    }
    System.out.println("EVICTED: for Process - " + page.processId + "  " + " pageNo - " + page.pageNo
        + "  page Count - " + page.count + "  lastReferenced - " + page.lastReferenced);
    page.processId = -1;
    page.processId = -1;
    page.pageNo = -1;
    pageArrayList.set(page.index, page);
    return pageArrayList;
  }
}
