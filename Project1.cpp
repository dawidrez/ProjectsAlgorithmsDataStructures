
#include <iostream>
using namespace std;
#pragma warning(disable:4996)

typedef struct {
 int id, size;

}file;
void swap(file arr[],int index1,int index2) {
    file tmp = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = tmp;
}

int partition2(file arr[], int end, int beg)
{
    int pivot = arr[beg].id;
    int a = 0;
    int beg1 = 0;
    int end1 = end;
    while (a >= 0)
    {
        int index1 = -1;
        int index2 = -1;
        for (;end1 - a >= beg;end1--)
        {
            if (arr[end1 - a].size >= pivot) {
                if (end1 - a > a)
                    index1 = end1 - a;

            }
        }
        for (;beg1 + a < end;beg1++)
        {
            if (arr[beg1 + a].size >= pivot) {
                if (beg1 + a < end - a)
                    index2 = beg1 + a;

            }
        }
        if (index1 != -1 && index2 != -1)
        {
            swap(arr, index1, index2);
            a++;
        }
        else
            break;
    }
    return a;
}
int partition(file arr[], int end, int beg)
{
    int pivot = arr[beg].size;
    int a=0;
    int beg1=0;
    int end1=end;
    while (a>=0)
    {
        int index1=-1;
        int index2=-1;
        for (;end1-a >= beg;end1--)
        {
            if (arr[end1-a].size<=pivot) {
                if (end1 - a > a)
                {
                    index1 = end1 - a;
                    break;
                }

            }
        }
        for (;beg1+a <= end;beg1++)
        {
            if (arr[beg1 + a].size > pivot) {
                if (beg1 + a < end - a)
                {
                    index2 = beg1 + a;
                    break;
                }
            }
        }
        if (index1 != -1 && index2 != -1)
        {
            swap(arr, index1, index2);
            a++;
        }
        else
            break;
    }
    return a;
}
//funckja sortuje rosnaco po rozmiarze pliku
void sort1(file arr[], int end,int beg)
{
    if (end == beg + 1)
    {
        if (arr[end].size < arr[end].size)
            swap(arr,end,beg);
    }
    else  if (end > beg)
    {
        int q = partition(arr, end, beg);
        
            sort1(arr, q, beg);
            sort1(arr, end, q+1);
        

    }

}
//funckja sortuje malejaco po id pliku
//bedzie wywolana 2 razy, po razie dla kazdego pendrajwa
void sort2(file arr[], int end,int beg)
{
    if (end == beg + 1)
    {
        if (arr[end].id > arr[beg].size)
            swap(arr, end, beg);
    }
    if (end > beg)
    {
        int q = partition2(arr, end, beg);
        if (beg + 1 < end)
        {
            sort2(arr, q, beg);
            sort2(arr, q + 1, end);
        }

    }

}
//funckja wczytuje dane na temat plikow
void load(file arr[],int number) {
    for (int i = 0;i < number;i++)
        scanf("%d %d" ,&arr[i].id, &arr[i].size);

}
//funckja oblicza ile plikow zmiesci sie na pendrajwie
int howMany(file arrFiles[], int numberFiles, int sizePendrive,int beg) {
    int amountFiles = 0;
    int memory=0;
        while (memory < sizePendrive)
        {
            memory += arrFiles[amountFiles].size;
            amountFiles++;
        }
        if (memory > sizePendrive)
        {
            amountFiles--;
        }
    
    return amountFiles;
}
int main()
{
    int numberFiles;
    scanf("%d", &numberFiles);
    int sizeP;
    scanf("%d", &sizeP);
    file* arr = (file*)malloc(sizeof(file) * numberFiles);
    load(arr,numberFiles);
    sort1(arr, numberFiles - 1,0);
    for (int i=0;i < numberFiles;i++)
    {
        printf("%d ", arr[i].id);
    }
    int amountF1= howMany(arr, numberFiles, sizeP,0);
    int amountF2 = howMany(arr, numberFiles, sizeP,amountF1);
    int amount = amountF1 + amountF2;
    sort2(arr, amountF1,0);
    sort2(arr, amountF2, amountF1);
    printf("%d \n 1: ", amount);
    int i = 0;
    for (;i < amountF1;i++)
    {
        printf("%d ", arr[i].id);
    }
    printf("\n 2:");
    for (;i < amountF2;i++)
    {
        printf("%d ", arr[i].id);
    }

    return 0;
}

