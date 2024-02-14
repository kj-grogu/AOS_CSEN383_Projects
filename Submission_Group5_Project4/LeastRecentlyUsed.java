import java.util.*;

class LeastRecentlyUsed {
  public static ArrayList<Page> execute(ArrayList<Page> pageArrayList) {
    Page page = new Page();
    int lru = (int) pageArrayList.get(0).lastReferenced;
    for (Page p : pageArrayList) {
      if (p.lastReferenced < lru) {
        page = p;
        lru = (int) p.lastReferenced;
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