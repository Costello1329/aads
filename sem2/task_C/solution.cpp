#include <iostream>
#include <vector>
#include <set>

using namespace std;


// Блок памяти. Хранит свой размер, индекс начала и пометку о том, занят ли он или нет.
struct Block {
	unsigned size;
	unsigned index;
	bool used;

	Block (unsigned size, unsigned index, bool used) {
		this->size = size;
		this->index = index;
		this->used = used;
	}
};

bool operator == (const Block &first, const Block &second) {
	return first.size == second.size && first.index == second.index && first.used == second.used;
}

bool operator != (const Block &first, const Block &second) {
	return !(first == second);
}

// Компараторы:
struct Compare {
	bool operator () (const Block &first, const Block &second) const {
		if (first.used != second.used)
			return second.used;

		else {
			if (first.size != second.size)
				return first.size > second.size;

			else 
				return first.index < second.index;
		}
	}
};

struct IndexCompare {
	bool operator () (const Block &first, const Block &second) const {
		return first.index < second.index;
	}
};


int main () {
	// Set, в котором хранятся блоки памяти, отсортированные занятости, потом по вместимости, потом по позиции.
	// Никак не отражает реальную структуру памяти. Только помогает за O(1) найти максимальный элемент.
	set<Block, Compare> memoryBySize;
	// Set, в котором хранятся блоки памяти, отсортированные по позиции.
	// Отражает реальную структуру памяти.
	set<Block, IndexCompare> memoryByIndex;

	unsigned N;
	cin >> N;
	memoryBySize.insert(Block(N, 0, false));
	memoryByIndex.insert(Block(N, 0, false));

	
	size_t M;
	cin >> M;

	Block nullBlock = Block(0, 0, 0);
	vector<Block> queries(M, nullBlock);

	for (size_t m = 0; m < M; m ++) {
		int q;
		cin >> q;

		if (q > 0) {
			// Получаем самый максимальный (первый по порядку блок):
			Block firstMax = *memoryBySize.begin();

			// Если места в памяти под запрос нет, либо все ячейки заняты - память выделить не можем.
			if (firstMax.size < q || firstMax.used) {
				cout << -1 << endl;
				continue;
			}

			// Ответ на запрос выделения - его позиция в 1-индексации:
			cout << (firstMax.index + 1) << endl;
			
			// Запоминаем ячейку, которую выделили под запрос:
			queries[m] = Block(q, firstMax.index, true);
			
			// Удаляем ячейку максимальную (первую по порядку) ячейку:
			memoryBySize.erase(firstMax);
			memoryByIndex.erase(firstMax);

			// Делим ячейку на 2 части, и добавляем левую часть (в ней мы выделили память) с пометкой о том что она занята:
			memoryBySize.insert(Block(q, firstMax.index, true));
			memoryByIndex.insert(Block(q, firstMax.index, true));

			// Добавляем правую часть (если мы заняли не всю найденную до этого ячейку) с пометкой о том что она свободна:
			if (firstMax.size - q > 0) {
				memoryBySize.insert(Block(firstMax.size - q, firstMax.index + q, false));
				memoryByIndex.insert(Block(firstMax.size - q, firstMax.index + q, false));
			}
		}

		else if (q < 0) {
			// Получаем элемент, на который ссылается запрос выделения, на который ссылается запрос освобождения.
			Block deletable = queries[- q - 1];

			// Если на запрос выделения, на который ссылается текущий запрос освобождения, ничего не было выделено, игнорируем запрос.
			if (deletable == nullBlock)
				continue;

			// Далее самое интересное. Если после освобождения, справо или слева от освобожденной ячейки будет другая свободная - их обязательно надо слить!
			// Получаем следующую за освобождаемой и предыдущую ячейку:
			set<Block, IndexCompare>::iterator iter = memoryByIndex.find(deletable);
			Block next = (std::next(iter, 1) != memoryByIndex.end() ? *std::next(iter, 1) : nullBlock);
			Block prev = (iter != memoryByIndex.begin() ? *std::prev(iter, 1) : nullBlock);

			// Удаляем освобождаемую ячейку.
			memoryBySize.erase(deletable);
			memoryByIndex.erase(deletable);

			// Удаляем следующую и запоминаем ее размер (предварительно проверяя, что это действительно свободная ячейка):
			if (next != nullBlock && !next.used) {
				memoryBySize.erase(next);
				memoryByIndex.erase(next);
				deletable.size += next.size;
			}

			// Удаляем предыдущую и запоминаем ее размер (предварительно проверяя, что это действительно свободная ячейка):
			if (prev != nullBlock && !prev.used) {
				memoryBySize.erase(prev);
				memoryByIndex.erase(prev);
				deletable.size += prev.size;
				// Если мы все же удалили предыдущую ячейку - нужно запомнить ее позицию, чтобы на нее вставить слитую.
				deletable.index = prev.index;
			}

			// Вставляем слитую ячейку на место освобожденной (с пометкой о том, что она свободна, конечно же):			
			deletable.used = false;
			memoryBySize.insert(deletable);
			memoryByIndex.insert(deletable);
		}
	}

	return 0;
}
