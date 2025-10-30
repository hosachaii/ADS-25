#include <stdio.h>

void print_array(int arr[], int n) {
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void merge(int *arr, int left, int mid, int right) {
	int n1 = mid - left + 1;
	int n2 = right - mid;

	int L[n1], R[n2];

	for (int i = 0; i < n1; i++)
		L[i] = arr[left + i];

	for (int j = 0; j < n2; j++)
		R[j] = arr[mid + 1 + j];

	int i = 0, j = 0, k = left;

	while (i < n1 && j < n2) {
		if (L[i] <= R[j])
			arr[k++] = L[i++];
		else
			arr[k++] = R[j++];
	}

	while (i < n1)
		arr[k++] = L[i++];

	while (j < n2)
		arr[k++] = R[j++];

	printf("Clubbing: ");
	for (int x = left; x <= right; x++)
		printf("%d ", arr[x]);
	printf("\n");
}

void merge_sort(int *arr, int left, int right) {
	if (left >= right)
		return;

	int mid = (left + right) / 2;

	printf("Dividing: ");
	for (int i = left; i <= right; i++)
		printf("%d ", arr[i]);
	printf("\n");

	merge_sort(arr, left, mid);
	merge_sort(arr, mid + 1, right);
	merge(arr, left, mid, right);
}

int main() {
	int arr[20] = {42, 17, 8, 33, 91, 56, 23, 11, 77, 19,
	               60, 4, 85, 31, 28, 90, 47, 64, 12, 39};
	int n = 20;

	printf("Original Array:\n");
	print_array(arr, n);
	printf("\n");

	merge_sort(arr, 0, n - 1);

	printf("\nSorted Array:\n");
	print_array(arr, n);

	return 0;
}

