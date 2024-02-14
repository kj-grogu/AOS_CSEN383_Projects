import java.util.*;

class FirstComeFirstServe {
  public static ArrayList<Page> execute(ArrayList<Page> pageArrayList) {
    Page page = pageArrayList.get(0);
    for (Page p : pageArrayList)
      if (p.broughtTime < page.broughtTime)
        page = p;

    System.out.println("EVICTED: for Process - " + page.processId + "  " + " pageNo - " + page.pageNo
        + "  page Count - " + page.count + "  lastReferenced - " + page.lastReferenced);
    page.processId = -1;
    page.pageNo = -1;
    pageArrayList.set(page.index, page);
    return pageArrayList;
  }
}