import java.util.*;

class Page {
  public int processId;
  public int index;
  public int pageNo;
  public double broughtTime;
  public double lastReferenced;
  public int count;

  public Page() {
    processId = -1;
    pageNo = -1;
  }

  public Page(int index) {
    processId = -1;
    pageNo = -1;
    this.index = index;
  }

  public boolean existsInMemory(ArrayList<Page> pl, int pid, int page_no) {
    for (Page t : pl)
      if (t.processId == pid && t.pageNo == page_no)
        return true;

    return false;
  }

  public boolean checkFreePages(ArrayList<Page> temp, int count) {
    for (Page t : temp) {
      if (t.processId == -1)
        count--;
      if (count == 0)
        return true;
    }
    return false;
  }

  public Page getFreePage(ArrayList<Page> pl) {
    for (int i = 0; i < pl.size(); i++)
      if (pl.get(i).processId == -1)
        return pl.get(i);
    return null;
  }

  public ArrayList<Page> updatePageList(Page p, ArrayList<Page> pl) {
    pl.set(p.index, p);
    return pl;
  }

  public void freeMemory(ArrayList<Page> pl, int pid) {
    for (Page p : pl) {
      if (p.processId == pid) {
        p.processId = -1;
        p.pageNo = -1;
      }
    }
  }

  public Page getPage(ArrayList<Page> pl, int pid, int page_no) {
    for (Page p : pl)
      if (p.processId == pid && p.pageNo == page_no)
        return p;
    return null;
  }

  public ArrayList<Page> getPageList() {
    ArrayList<Page> temp = new ArrayList<>();
    for (int i = 0; i < Main.PAGE_LIST_SIZE; i++)
      temp.add(new Page(i));

    return temp;
  }

  public void printMemoryMap(ArrayList<Page> pl) {
    System.out.println("Memory map is");
    for (Page p : pl)
      if (p.processId == -1)
        System.out.print(".|");
      else
        System.out.print(p.processId + "|");
    System.out.println("\n");
  }

  public int getNextPageNumber(int curr_page_no, int max_page_size) {
    Random rand = new Random();
    int x = rand.nextInt(10);
    if (x < 7) {
      x = curr_page_no + (rand.nextInt(3)) - 1;
    } else {
      x = rand.nextInt(max_page_size);
      while (Math.abs(x - curr_page_no) <= 1)
        x = rand.nextInt(max_page_size);
    }
    if (x < 0)
      x = 0;
    if (x >= 100)
      x = max_page_size - 1;
    return x;
  }

  public void displayPageList(ArrayList<Page> temp) {
    int counter = 0;
    for (int i = 0; i < temp.size(); i++) {
      Page t = temp.get(i);
      if (t.count == 0) {
        System.out.print("Page " + counter + " .|");
        counter++;
      } else {
        System.out.println();
        if (t.processId > 0)
          System.out.print("pid: " + t.processId + "\t" + "count: " + t.count + "\t"
              + String.format("last used: %.1f| ", t.lastReferenced));
        else
          System.out.print("\t" + "count: " + t.count + "\t" + String.format("last used: %.1f| ", t.lastReferenced));
        if (i % 10 == 0)
          System.out.println(" ");
      }
    }
  }

}