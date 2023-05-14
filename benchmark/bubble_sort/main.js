function swap(arr, xp, yp) {
	var temp = arr[xp];
	arr[xp] = arr[yp];
	arr[yp] = temp;
}

// An optimized version of Bubble Sort
function bubbleSort(arr, n) {
	var i, j;
	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				swap(arr, j, j + 1);
			}
		}
	}
}

/* Function to print an array */
function printArray(arr, size) {
	var i;
	for (i = 0; i < size; i++) document.write(arr[i] + " ");
	document.write("\n");
}

// Driver program to test above functions
var arr = [5, 1, 4, 2, 8];
var n = 5;
document.write("UnSorted array: \n");
printArray(arr, n);

bubbleSort(arr, n);
document.write("Sorted array: \n");
printArray(arr, n);
