#include "stdafx.h" 
#include <iostream> 
#include <vector>
#include <algorithm> 
#include <numeric>
#include <iterator>
#include "mpi.h"
#include <cstdlib>



using namespace std;

void vectorToArray(vector<int> Vector, int Array[], int n)
{

	for (int i = 0; i < n; i++) Array[i] = Vector[i];
}

void arrayToVector(int Array[], vector<int> Vector, int n)
{

	for (int i = 0; i < n; i++) Vector[i] = Array[i];
}

void bubbleSort(int arr[], int size)

{

	int tmp;
	for (int i = 0; i < size - 1; i++) {

		for (int j = 0; j < size - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {

				// ������ �������� ������� tmp = arr[j];

				arr[j] = arr[j + 1]; arr[j + 1] = tmp;

			}

		}

	}
}

int main(int argc, char* argv[])
{

	ostream_iterator< int > output(cout, " ");

	int nTasks, rank; MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &nTasks); MPI_Comm_rank(MPI_COMM_WORLD, &rank); MPI_Status status;

	vector <int> origSeq(32);//������������ ������������������

	vector <int> lessPart;// ��� ���������� �� ����� ����� ������������������ �� ������� �� ����������

	vector <int> greatPart;


	int host;//������� �������

	//------------- ���������� ���������-------------

	MPI_Comm old_comm, new_comm;

	int ndims, reorder, periods[3], dim_size[3], rankDec, coords[3]; old_comm = MPI_COMM_WORLD;

	ndims = 3; // 3D matrix/grid ����������� ������ ������������

	dim_size[0] = 2; // rows ������ ����������� ndims, ����������� ������� �� ������� �����������

	dim_size[1] = 2; // columns dim_size[2] = 2; // columns
	periods[0] = 0; // row periodic (each column forms a ring)	������, �����������

	//������������� ������ �� ������� �����������

		periods[1] = 1; // columns non-periodic */ /* � ������ ������ ��� ����������

	//����������, �������� �� ������ ������� � ������ ������ ��� ������� ������� � ��������� ��������� � ���� ������ ��� ������� ��������������.

		periods[2] = 1;

	reorder = 1; // allows processes reordered for efficiency */ /*��� �������� �����
	//��������� "������", ������� ��������� ������ ������� ��������� ���������.

		MPI_Cart_create(old_comm, ndims, dim_size, periods, reorder,

		&new_comm);//MPI_GRAPH ��� ��������� �����; MPI_CART ��� ���������� ���������;

	MPI_Comm_rank(new_comm, &rankDec); //-----------------------------------------------


	int origSeqAr[32]; int partSeqAr[4]; int N = 0;

	int lessSize, greatSize;//������� ������������ �������� int accSize;//�����������

	int sendRank; //���� �������� ��� ����������� int newSize;

	int *sendArr, *acceptArr; int inf[2];
	int sum = 0;

	//-------------------------------------------�������� ������ ������������������ �
	//�������� ��������-------------------------- -

	if (rank == 0)

	{

		//origSeq = { 4,1,9,19,5,3,44,109,0,6,77,14,28,37,42,53,11,1,94,78,65,33,2,51,50,61,80,46,6,30,60,71 };

		origSeq = { 1, 12, 91, 19, 51, 33, 42, 109, 0, 6, 7, 141, 28, 3, 2, 53, 11, 1, 94, 78, 65, 33, 2, 51, 50, 61, 80, 46, 6, 30, 60, 71 };

		//origSeq = { 33,22,11,15,2,5,37,77,14,41,76,67,20,30,41,35,79,23,711,7,8,33,52,5,4,13,31,3,6,86,4,9 };

		cout << "\n Original sequence: "; copy(origSeq.begin(), origSeq.end(), output); cout << endl;

		//	��������� ����� ��������� � origSeq

		int total = accumulate(origSeq.begin(), origSeq.end(), 0); cout << "\n The total of the elements in original sequence: "

			<< total; cout << endl;

		//	������� �������������� ��� ������ �������� �������� 
		host = total / origSeq.size();
		cout << "\n The average of the total: "
			<< host;

		cout << endl << endl;

		// ������ � ������ vectorToArray(origSeq,origSeqAr,32);

		//��������� ������(�� 4)


		MPI_Scatter(origSeqAr, 4, MPI_INT, &partSeqAr, 4, MPI_INT, 0, MPI_COMM_WORLD);

		// ��������� ������� �������

		MPI_Bcast(&host, 1, MPI_INT, 0, MPI_COMM_WORLD);

	}
	else

	{

		//��������� ��������� ������

		MPI_Scatter(origSeqAr, 8, MPI_INT, &partSeqAr, 4, MPI_INT, 0, MPI_COMM_WORLD);

		// ����� �������� ��������
		MPI_Bcast(&host, 1, MPI_INT, 0, MPI_COMM_WORLD);

	}

	//--------------------------------------------------------------------------------
	---------------------------------------------- -

		MPI_Barrier(MPI_COMM_WORLD);

	//����������� ������ � ������������ 
	acceptArr = new int[4];

	for (int i = 0; i < 4; i++) acceptArr[i] = partSeqAr[i];
	newSize = 4; //������ �������

	while (N != 3)
	{

		MPI_Barrier(MPI_COMM_WORLD);

		if (rank == 0)

			cout << "\n----------------ITERATION " << N + 1 << "----------------" <<
			endl;

		MPI_Barrier(MPI_COMM_WORLD);

		cout << "From rank " << rankDec << ": "; for (int i = 0; i < newSize; i++)

			cout << acceptArr[i] << " "; cout << endl; MPI_Barrier(MPI_COMM_WORLD);

		//����� �������� �������� ���� �� ������� �������� if (N!=0)

		{

			MPI_Cart_coords(new_comm, rankDec, 3, coords); if (coords[N] == 1)

			{

				MPI_Recv(&inf, 2, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
					&status);

				for (int i = 0; i < newSize; i++) sum += acceptArr[i];

				// ������� �������������� ��������� ���. ����� host = (sum + inf[0]) / (inf[1] + newSize);

				//�������� host coords[N] = 0;

				MPI_Cart_rank(new_comm, coords, &sendRank); MPI_Send(&host, 1, MPI_INT, sendRank, 99, MPI_COMM_WORLD); MPI_Cart_coords(new_comm, rankDec, 3, coords);

			}

			else
			{

				for (int i = 0; i < newSize; i++) sum += acceptArr[i];

				inf[0] = sum; inf[1] = newSize;

				//


				coords[N] = 1;

				MPI_Cart_rank(new_comm, coords, &sendRank); MPI_Send(&inf, 2, MPI_INT, sendRank, 99, MPI_COMM_WORLD); MPI_Cart_coords(new_comm, rankDec, 3, coords);

				//����� ���. �������
				MPI_Recv(&host, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,

					&status);

			}
			sum = 0;

		}

		//�������� ������ �� 2 ������� - ������� ������ �������� �������� for (int i = 0; i < newSize; i++)

		{

			if (acceptArr[i] >= host) greatPart.push_back(acceptArr[i]);

			else

				lessPart.push_back(acceptArr[i]);

		}

		delete[]acceptArr; //������ ������, ����� ����� �������� ������ ����� ������ ��� ����� ������

		MPI_Barrier(MPI_COMM_WORLD); if (rank == 0)

			cout << "\nAfter division:" << endl; MPI_Barrier(MPI_COMM_WORLD);

		//�����	�� ����� ��������

		cout << "From rank " << rank << " : less part: "; copy(lessPart.begin(), lessPart.end(), output); cout << " great part: "; copy(greatPart.begin(), greatPart.end(), output); cout << endl;

		MPI_Barrier(MPI_COMM_WORLD);

		// ������ �������� ��� �������� lessSize = lessPart.size(); greatSize = greatPart.size();

		if (rank == 0)

			cout << "\nAfter transfer:" << endl;

		MPI_Barrier(MPI_COMM_WORLD);

		//----------------------------------------------����� �������	� �������

		//������	���� ���.�������---------------------- -
			//�� ����� � ����� 1 - ������� �����, 0 - �������

			MPI_Cart_coords(new_comm, rankDec, 3, coords);

		if (coords[N] == 1)
		{

			//---------------------����� �������� �������---------------------

			//���������	����� ��������

			MPI_Recv(&accSize, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
				&status);

			//������� ������ ��� ����� ������ newSize = accSize + greatSize; acceptArr = new int[newSize];

			for (int i = 0; i < greatSize; i++) acceptArr[i] = greatPart[i];

			for (int i = greatSize; i < newSize; i++) MPI_Recv(&acceptArr[i], 1, MPI_INT, MPI_ANY_SOURCE, 99,

				MPI_COMM_WORLD, &status);


			cout << "From rank " << rank << ": "; for (int i = 0; i < newSize; i++)

				cout << acceptArr[i] << " "; cout << endl;

			//---------------------�������� ������� �������-----------------

			//��������� ����� �������� ������� coords[N] = 0;
			MPI_Cart_rank(new_comm, coords, &sendRank);

			MPI_Send(&lessSize, 1, MPI_INT, sendRank, 99, MPI_COMM_WORLD);

			MPI_Cart_coords(new_comm, rankDec, 3, coords);
			//��������� ������

			for (int i = 0; i < lessSize; i++) MPI_Send(&lessPart[i], 1, MPI_INT, sendRank, 99,
				MPI_COMM_WORLD);

		}

		else //���� 0
		{

			//---------------------�������� �������� �������-----------------

			//��������� ����� �������� ������� coords[N] = 1;

			MPI_Cart_rank(new_comm, coords, &sendRank);

			MPI_Send(&greatSize, 1, MPI_INT, sendRank, 99, MPI_COMM_WORLD);
			MPI_Cart_coords(new_comm, rankDec, 3, coords);

			for (int i = 0; i < greatSize; i++) MPI_Send(&greatPart[i], 1, MPI_INT, sendRank, 99,
				MPI_COMM_WORLD);


				//---------------------����� �������� �������---------------------

				//���������	����� ��������

				MPI_Recv(&accSize, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
				&status);

			//������� ������ ��� ����� ������ newSize = accSize + lessSize; acceptArr = new int[newSize];

			for (int i = 0; i < lessSize; i++) acceptArr[i] = lessPart[i];

			for (int i = lessSize; i < newSize; i++) MPI_Recv(&acceptArr[i], 1, MPI_INT, MPI_ANY_SOURCE, 99,
				MPI_COMM_WORLD, &status);

				cout << "From rank " << rank << ": "; for (int i = 0; i < newSize; i++)

				cout << acceptArr[i] << " "; cout << endl;

		}

		//-------------------------------------------------------------------------

		------------------------------------------------
			greatPart.clear();

		lessPart.clear(); N++;
	}

	MPI_Barrier(MPI_COMM_WORLD);


	//�������� ���

	if (rankDec == 0) {


		int orderedSeq[32];

		int count[8]; //���������� ��������� �� ������� ���������� int position=newSize; //���������� ���������

		int tmp;

		//����������� bubbleSort(acceptArr, newSize);

		for (int i = 0; i < newSize; i++) orderedSeq[i] = acceptArr[i];

		count[0] = newSize;

		for (int i = 0; i < 7; i++)

		{
			MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,

				&status);

			count[status.MPI_SOURCE] = tmp;

		}
		MPI_Barrier(MPI_COMM_WORLD);

		//��������� � ������������ for (int i = 1; i < 8; i++)

		{

			for (int j = 0; j < count[i]; j++)
			{

				MPI_Recv(&tmp, 1, MPI_INT, i, 99, MPI_COMM_WORLD, &status); orderedSeq[position] = tmp;

				position++;
			}

		}

		cout << endl << endl << "ORDERED SEQUENCE: "; for (int i = 0; i < 32; i++)

			cout << orderedSeq[i] << " "; cout << endl;

	}

	else
	{

		MPI_Request request;

		MPI_Send(&newSize, 1, MPI_INT, 0, 99, MPI_COMM_WORLD); MPI_Barrier(MPI_COMM_WORLD);
		vector <int> forSort(newSize);

		//����������� �������������� bubbleSort(acceptArr,newSize);

		for (int i = 0; i < newSize; i++)
			MPI_Isend(&acceptArr[i], 1, MPI_INT, 0, 99, MPI_COMM_WORLD,

			&request);

	}

	MPI_Barrier(MPI_COMM_WORLD); MPI_Finalize(); system("pause 0");

	return 0;
}
