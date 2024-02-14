import java.util.*;

class LeastFrequentlyUsed {
  public static ArrayList<Page> execute(ArrayList<Page> pageArrayList) {
    Page page = new Page();
    int min_count = pageArrayList.get(0).count;
    for (Page p : pageArrayList) {
      if (p.count < min_count) {
        page = p;
        min_count = p.count;
      }
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