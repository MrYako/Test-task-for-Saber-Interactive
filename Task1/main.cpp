#include<string>
#include<iostream>
#include<map>

struct ListNode {

ListNode * prev = nullptr;

ListNode * next = nullptr;

ListNode * rand = nullptr; // указатель на произвольный элемент данного списка либо NULL

std::string data;

};

class List {
public:

void Serialize    (FILE * file); // сохранение в файл (файл открыт с помощью fopen(path, "wb"))
void Deserialize (FILE * file); // загрузка из файла (файл открыт с помощью fopen(path, "rb"))
void push_back(ListNode* node);
void print_list();

private:

ListNode *        head;

ListNode *        tail;

int count;

} ;

void List::push_back(ListNode* node)
{
    if(count == 0)
    {
        head = node;
    }
    else
    {
        tail->next = node;
        node->prev = tail;
    }
    tail = node;
    ++count;
}

void List::print_list()
{
    if(count == 0) return;
    std::cout << "List size : " << count << std::endl;
    for(ListNode* node = head; node != nullptr; node = node->next)
    {
        std::cout << "Current ptr:"<< std::hex << node << " | prev ptr: " 
        << std::hex << node->prev << " | next ptr: " << std::hex << node->next << 
        " | rand ptr: " << std::hex << node->rand << " | data: " << node->data << std::endl;
    }
}

void List::Serialize (FILE * file)
{
    fwrite(&count, sizeof(count), 1, file);
    for(ListNode* node = head; node != nullptr; node = node->next)
    {
        fwrite(&node, sizeof(node), 1, file);
        fwrite(&node->prev, sizeof(node->prev), 1, file);
        fwrite(&node->next, sizeof(node->next), 1, file);
        fwrite(&node->rand, sizeof(node->rand), 1, file);

        auto dataSize = node->data.size();

        fwrite(&dataSize, sizeof(dataSize), 1, file);
        fprintf(file, "%s", node->data.data());
    }
}

void List::Deserialize(FILE* file)
{
    count = 0;
    std::map<ListNode*, ListNode*> OldNewNode;
    std::map<ListNode*, ListNode*> OldRandNode; 
    int newCount;
    
    fread(&newCount, sizeof(newCount), 1, file);
    for (int i = 0; i < newCount; ++i)
    {
        ListNode *ptr, *prev_ptr, *next_ptr, *rand_ptr;
        std::string::size_type dataSize;
        fread(&ptr, sizeof(ptr), 1, file);
        fread(&prev_ptr, sizeof(prev_ptr), 1, file);
        fread(&next_ptr, sizeof(next_ptr), 1, file);
        fread(&rand_ptr, sizeof(rand_ptr), 1, file);

        fread(&dataSize, sizeof(dataSize), 1, file);
        char text[dataSize];

        fread(text, sizeof(char), dataSize, file);
        auto* new_ptr = new ListNode{nullptr, nullptr, nullptr, text};
        push_back(new_ptr);
        OldNewNode.insert(std::make_pair(ptr,new_ptr));
        OldRandNode.insert(std::make_pair(ptr,rand_ptr));
    }

    for (auto & iterator : OldRandNode)
    {
        if (iterator.second != nullptr)
        {
            auto currentNewtmp = OldNewNode.find(iterator.first)->second;
            currentNewtmp->rand = OldNewNode.find(iterator.second)->second;
        }
    }
}


int main()
{
    List list;

    auto node1 = new ListNode;
    node1->data = "data1";
    auto node2 = new ListNode;
    node2->data = "data2";
    auto node3 = new ListNode;
    node3->data = "data3";
    auto node4 = new ListNode;
    node4->data = "data4";

    node1->rand = node2;
    node2->rand = node3;
    node4->rand = node1;

    list.push_back(node1);
    list.push_back(node2);
    list.push_back(node3);
    list.push_back(node4);
    
    FILE* output = fopen("input.txt", "wb");
    list.Serialize(output);
    fclose(output);

    List newList;
    FILE* input = fopen("input.txt", "rb");
    newList.Deserialize(input);
    fclose(input);

    //list.print_list();
    //newList.print_list();
}