/* Supertos Industries ( 2012 - 2024 ) 
 * Author: Supertos, 2024
 *
 * MAI Operating Systems course. Exercise 2.
 */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


typedef struct ArrayPassInfo ArrayPassInfo;
struct ArrayPassInfo {
	size_t mask;
	size_t offset;
	size_t dist;
	size_t start;
	size_t end;
	size_t arrSize;
	int* arr;
};

void swapInt( int* a, int* b ) {
	int temp = *b;
	*b = *a;
	*a = temp;
}

/* One array pass */
void* sortArrayPassFraction( void* infoP ) {
	if( !infoP ) return NULL;
	
	ArrayPassInfo* info = (ArrayPassInfo*)infoP;
	
	size_t mask = info->mask;
	size_t offset = info->offset;
	size_t dist = info->dist;
	size_t arrSize = info->arrSize;
	size_t start = info->start;
	size_t end = info->end;
	int* arr = info->arr;
	
	for( size_t i = start; i < end; ++i ) {
		if( ( i & mask ) != offset ) continue; // Skip elements that are in even or odd blocks, to avoid overlapping.
		if( i + dist < arrSize && arr[i] > arr[i + dist] ) swapInt( &arr[i], &arr[i + dist] ); 
	}
	
	free( infoP ); // Allocated in sortArrayPass
	
	return NULL;
}

void sortArrayPass( size_t mask, size_t offset, size_t dist, size_t threads, int arr[], size_t arrSize ) {
	pthread_t* threadlist = (pthread_t*)malloc( sizeof(pthread_t) * threads );
		
    size_t elementsLeft = arrSize - dist;
    size_t start = 0;
		
	for( size_t threadNo = threads; threadNo > 0; --threadNo ) {
		size_t elements = elementsLeft / threadNo;
			
		elementsLeft -= elements;
			
		ArrayPassInfo* info = (ArrayPassInfo*)malloc( sizeof(ArrayPassInfo) );
		*info = (ArrayPassInfo){
			.mask = mask,
			.offset = offset,
			.dist = dist,
				
			.start = start,
			.end = start + elements,
			.arrSize = arrSize,
			.arr = arr
		};
		
		pthread_create( &threadlist[threadNo - 1], NULL, sortArrayPassFraction, info );
		start += elements;
	}
	
	for( size_t threadNo = threads; threadNo > 0; --threadNo ) {
		pthread_join( threadlist[threadNo - 1], NULL );
	}
	free( threadlist );
}

void batcherSort( int arr[], size_t size, size_t threads ) {
	size_t batcherSize = pow( 2, log( size ) + 1);
	
	for (int mask = batcherSize; mask > 0; mask /= 2) {
		sortArrayPass( mask, 0, mask, threads, arr, batcherSize );
		sortArrayPass( mask, mask, batcherSize - mask, threads, arr, batcherSize );
    }
}

int main() {
	size_t s = 4;
	int arr[4] = {8, 76, 54, 19};
	
	for( size_t i = 0; i < s; i++ ) {
		printf( "%d ", arr[i] );
	}
	batcherSort( arr, s, 1 );
	
	for( size_t i = 0; i < s; i++ ) {
		printf( "%d ", arr[i] );
	}
		
	return 0;
	
}