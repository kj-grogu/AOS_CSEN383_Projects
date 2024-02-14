import java.util.*;

class MostFrequentlyUsed{
  public static ArrayList<Page> execute(ArrayList<Page> pageArrayList){
    Page page = new Page();
    int max = pageArrayList.get(0).count;
    for(Page p : pageArrayList){
      if(p.count > max){
        page = p;
        max = p.count;
      }
    }
    page.processId = -1;
    page.pageNo = -1;
    pageArrayList.set(page.index,page);
    return pageArrayList;
  }
}