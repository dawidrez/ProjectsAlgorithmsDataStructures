#include <iostream>
using namespace std;
#include <string>
struct Node {
	Node** children;
	Node* parent;
	int* keys;
	bool isLeaf;
	int counter;
};

struct Data {
	Node* root;
	int order;
};

//funckja sluzy do zapisania drzewa w postaci narzuconej w zzadaniu
void save(Node* act) {    
	for (int i = 0;i < act->counter - 1;i++) {
		if (!act->isLeaf)   
		{ 
			cout << "( ";
			save(act->children[i]);  //jezeli nei jest lisciem to wyswielamy zawartosc i--tego syna
			cout << ") ";
		}
		cout << act->keys[i] << " ";
	}
	if (!act->isLeaf)
	{
		cout << "( ";                           //dzieci jest o 1 wiecej niz kluczy 
		save(act->children[act->counter - 1]);
		cout << ") ";
	}
}

void printNode(Node* act) {
	for (int i = 0;i < act->counter - 1;i++) {
		if (!act->isLeaf)                    //jezeli nie jest lisciem to wywolujemy funckje dla i-tego dziecka
			printNode(act->children[i]);
		cout << act->keys[i] << " ";
	}
	if (!act->isLeaf)         //dzieci jest o 1 wiecej niz kluczy 
		printNode(act->children[act->counter - 1]);
}

//zwalniamy wszsytkie wezly alokowane dynamicznie
void freeMemory(Node* act) {
	for (int i = 0;i < act->counter - 1;i++) {
		if (!act->isLeaf)
			freeMemory(act->children[i]);
		else
		{
			delete [] act->keys;
			delete [] act->children;
			delete act;
			return;
		}
	}
	delete [] act->keys;
	delete [] act->children;
	delete act;
}

//tworzenie nowego wezla z domyslnymi wartosciami 
Node* createNode(int order) {
	Node* tmp = new Node;
	tmp->keys = new int[order * 2 - 1];
	tmp->children = new Node * [2 * order];
	tmp->counter = 1;
	tmp->parent = NULL;
	for (int i = 0;i < order * 2;i++)
		tmp->children[i] = NULL;
	return tmp;
}

//funckja szuka wartosci, ktora zostala podana przez uzytkownika
void lookFor(int value, Node* node) {
	int size = node->counter;
	for (int i = 0;i < size - 1;i++)
	{
		if (value == node->keys[i]) {
			cout << value << " +" << endl;
			return;
		}
		if (value < node->keys[i]) {
			if (!node->isLeaf)
			{
				lookFor(value, node->children[i]);
				return;
			}
			else {
				cout << value << " -" << endl;
				return;
			}
		}
	}
	if (!node->isLeaf)
		lookFor(value, node->children[size - 1]);
	else
		cout << value << " -" << endl;
	return;
}

//fucnkaj przesuwa klucze oraz wskanziki na dzieci w tablicach o jedno miesjce w prawo od miejsca poczatkowego-beg
void replaceKeysChildren(Node* node, int beg, int end, int order) {

	int temp, temp2;
	temp2 = 0;
	temp = node->keys[beg];
	Node* temp_ptr, * temp_ptr2;
	temp_ptr = node->children[beg + 1];
	for (int i = beg + 1;i < end;i++)
	{
		temp_ptr2 = node->children[i + 1];
		node->children[i + 1] = temp_ptr;
		temp_ptr = temp_ptr2;
		temp2 = node->keys[i];
		node->keys[i] = temp;
		temp = temp2;
	}
}

//funckja przepisuje klucze oraz wskanziki na dzieci w tablicach z wezla source do wezla dest
void rewrite(Node* source, Node* dest, int order) {
	int j = 0;
	dest->counter = 0;
	for (int i = order;i < 2 * order - 1;j++, i++) //przepisujemy 2 polowe tablic wezla source do welza dest
	{
		dest->keys[j] = source->keys[i];
		dest->children[j] = source->children[i];
		if (dest->children[j] != NULL) //jezeli wezly ktore przenosimy nei sa Nullami to ustawiamy im rodzica
			dest->children[j]->parent = dest;
		dest->counter++;
		source->children[i] = NULL;
	}
	dest->children[j] = source->children[(2 * order) - 1];
	if (dest->children[j] != NULL)
		dest->children[j]->parent = dest;     //to samo co w petli, tylko bez przepisywania klucza
	source->children[2 * order - 1] = NULL;
	dest->counter++;
	dest->parent = source->parent;
}

//funckja dzieli maskymalna tablice na 2 czesci, jezeli jestesmy w korzeniu, to ustawiamy nowy korzen
Node* split(Node*& node, Data* data) {
	Node* newNode1 = createNode(data->order);
	newNode1->isLeaf = node->isLeaf;
    if (node->parent == NULL)
	{
		Node* newNode2 = createNode(data->order);
		data->root = newNode2;
		node->parent = newNode2;
		newNode2->isLeaf = false;
		newNode2->counter = 2;//liczba dzieci bedzie rowna 2
		int value = node->keys[data->order - 1];//1 klucz w korzeniu to bedzie srodkowy klucz maksymalnego wezla
		newNode2->keys[0] = value;
		newNode2->children[0] = node;
		newNode2->children[1] = newNode1;
		rewrite(node, newNode1, data->order); //przepisanie wezlow z maksymalnego do nowego 
		node->counter = data->order;
		return newNode2;
	}
	else
	{
		rewrite(node, newNode1, data->order);
		Node* parent = node->parent;
		int i = 0;
		for (;parent->children[i] != node;i++); //znajdujemy miejsce wskaznika na nasz maksymalny wezel w rodzicu 
		replaceKeysChildren(parent, i, parent->counter, data->order); //przenosimy wskazniki oraz klucze o 1 miejsce 
		parent->keys[i] = node->keys[data->order - 1];
		parent->children[i + 1] = newNode1;
		parent->counter++;
		node->counter = data->order;
		return parent;
	}

}

//dodajemy wartosc do drzewa
void add(int value, Node* node, Data* data) {
	if (node->counter == 2 * data->order)
	{                                  //sprawdzamy, czy wezel nei jest maksymalny
		node = split(node, data);
	}
	bool flag = false;
	//zainicjowanie 1 wartosci w drzewie
	if (node->counter == 1)
	{
		node->counter++;
		node->keys[0] = value;
	}
	else
		for (int i = 0;i < node->counter;i++, flag = false)
		{
			if (i == node->counter - 1)
				flag = true;             //szukamy tak dlugo,az klucz w wezle bedzie wiekszy lub rowny dodawanej wartosci
			if (value <= node->keys[i])  //jezeli takiej nei znajdziemy to dodajemy na koniec
				flag = true;
			if (flag)
			{
				if (node->isLeaf)
				{
					replaceKeysChildren(node, i, node->counter, data->order); //jezeli jest to lisc to przesuwamy wartosci i dodajemy w odpowiednim miejscu
					node->counter++;
					node->keys[i] = value;
				}
				else
					add(value, node->children[i], data);  //w przeciwnym wypadku uruchamiamy funckje dla odpowiedniego wezla
				return;
			}
		}
}

void loadNode(int order, Node* act) {
	string com;
	char first_char;
	act->counter = 0;
	while (cin >> com) {
		first_char = com[0];
		if (first_char == '(')
		{
			Node* newNode = createNode(order);
			newNode->parent = act;
			act->children[act->counter] = newNode;  //schodzimy w dol drzewa 
			act->counter++;
			loadNode(order, newNode);
		}
		else if (first_char == ')')
		{
			if (act->children[0] == NULL) //jezeli nie ma potomkow to jest lisciem
			{
				act->isLeaf = true;
				act->counter++;  //puste wezly w moim kodzie maja counter ustawiony na 1
			}
			else
				act->isLeaf = false;
			return;
		}
		else if (int(first_char) >= 48 && int(first_char) < 58 || int(first_char) == 45) //jezeli jest liczba
		{
			int key = atoi(com.c_str());
			if (act->children[0] == NULL) //jestesmy w lisciu(ma juz klucz, a nie ma dziecka)
				act->counter++;
			act->keys[act->counter - 1] = key;
		}
		else
		{
			cout << "Nie poprawne dane";
			return;
		}
	}
}

int main()
{
	char comand;
	int value;
	char com;
	Data* data = new Data;
	while (cin >> comand) {
		if (comand == 'I') {
			cin >> value;
			data->order = value;
			data->root = createNode(data->order);
			data->root->isLeaf = true;
		}
		if (comand == 'L') {
			cin >> value;
			data->order = value;
			data->root = createNode(data->order);
			cin >> com;
			loadNode(data->order, data->root);
		}
		if (comand == '?') {
			cin >> value;
			lookFor(value, data->root);
		}
		if (comand == 'A') {
			cin >> value;
			add(value, data->root, data);
		}
		if (comand == 'X')
			break;
		if (comand == 'P')
			printNode(data->root);
		if (comand == 'S')
		{
			cout << data->order << endl;
			cout << "( ";
			save(data->root);
			cout << " )";

		}
	}
	freeMemory(data->root);
	delete data;
	return 0;
}

