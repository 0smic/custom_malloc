#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

/*
I implemented this allocated as stack based, 
You could use mmap and get memory from os and update the freed_chunks chunks[0].start to the result of mmap
It should work fine ig, also update according to that like freeing the memory to the os use the munmap
*/

#define HEAP_CAPACITY 10000
#define CHUNK_LIST_CAP 100

#define UNIMPLEMENTED \
	do { \
		fprintf(stderr,"%s:%d: TODO: %s is not implemented yet\n", \
				__FILE__,__LINE__,__func__); \
		abort(); \
	} while(0)

typedef struct {
	void *start;
	size_t size;
}Chunk;


typedef struct {
	size_t count;
	Chunk chunks[CHUNK_LIST_CAP];
}Chunk_list;


char heap[HEAP_CAPACITY] = {0};
Chunk_list tmp_chunks = {0};

Chunk_list alloced_chunks = {0};
Chunk_list freed_chunks = {
	.count = 1,
	.chunks = {
		[0] = {.start = heap, .size = sizeof(heap)}
	},
};

/*Dump element in the list provided, ig I'm stupid to explain this part to you*/
void dump_chunk_list(const Chunk_list *list){
	printf("\n Chunks : %lu",(unsigned long)list->count);
	for(size_t i = 0; i<list->count; i++){
		printf("\n Start : %p -- size %lu",list->chunks[i].start, (unsigned long)list->chunks[i].size);
	}
}


/* find the chunk index from the list, it's linear, you can update it, to a bineary search if you want*/
int chunk_list_find(const Chunk_list *list, void *ptr){
	for (size_t i = 0; i<list->count; i++){
		if(list->chunks[i].start == ptr){
			return (int)i;
		}
	}
	return -1;
}


/* It append the chunk in the list at the tail and then it reposition the chunk according to the pointer, it's in ascending*/
void chunk_list_insert(Chunk_list *list, void *start, size_t size){
	assert(list->count < CHUNK_LIST_CAP);
	list->chunks[list->count].start = start;
	list->chunks[list->count].size = size;
	list->count++; 
	
	for(size_t i = list->count - 1; i>0 && list->chunks[i].start < list->chunks[i - 1].start; --i){
		Chunk tmp = list->chunks[i];
		list->chunks[i] = list->chunks[i-1];
		list->chunks[i-1] = tmp;
	}
}

/* It remove the chunk from the list, 
by reaching the index of the chunk and swap it with the adjacent chunk and doing it until chunk reach the tail
and decreament of the count of the list*/
void chunk_list_remove(Chunk_list *list, size_t index){
	assert(index<=list->count);
	for(int i = index; i<list->count -1; i++){
		list->chunks[i] = list->chunks[i + 1];
	}
	list->count--;	
}

/*
It merger the fragmented chunks in the freed_chunks, we merge chunk which are consecutive in the freed_chunks
It taked another list which is a tmp list and set the count to zero
iteration through the freed_chunk
  count == 0
      so if the count is zero we append the frist element of free_chunks(src) into the tmp_chunk(dest)
  count !=0
  	we store the previous chunk into a var top_chunk
   	then we add top_chunk + size of top_chunk if that is equal to the current chunk of the freed_chunk( meaning it is consecutive)
        if consecutive
           we add the current freed_chunk chunk size to the top_chunk size and store it in top_chunk ( meaning we merged)
	if not consective
 	   we append the chunk into the tmp chunk list

      

	
*/

void chunk_list_merge(Chunk_list *dst, Chunk_list *src){
	dst->count = 0;
	for (size_t i = 0; i < src->count; i++) {
		const Chunk chunk = src->chunks[i];

		if (dst->count == 0) {
			chunk_list_insert(dst, chunk.start, chunk.size);
			continue;
		}

		Chunk *top_chunk = &dst->chunks[dst->count - 1];

		if ((char *)top_chunk->start + top_chunk->size == chunk.start) {
			// Merge the two chunks
			top_chunk->size += chunk.size;
		} else {
			chunk_list_insert(dst, chunk.start, chunk.size);
		}
	}
}




/*
This function allocated memory for you.
Frist it try to merge fragmented chunk in the freed memory, Ik it is expensive you can remove if you want.
next we iterate through the freed chunks and try to find the suitable chunk.
There is a chance that the chunk we find have more space than we needed.
 So we use the necessary size we want and the rest of them will append in the freed chunk list (there by sorting and sort of things).
*/
void *heap_alloc(size_t size){
	if(size == 0 || size <0) return NULL;
	chunk_list_merge(&tmp_chunks, &freed_chunks);
	freed_chunks = tmp_chunks;
	//allocate space in heap
	for (size_t i =0;i<freed_chunks.count;i++){
		Chunk chunk = freed_chunks.chunks[i];
		if(chunk.size >= size){
			chunk_list_remove(&freed_chunks,i);

			const size_t tail_size = chunk.size - size;
			chunk_list_insert(&alloced_chunks, chunk.start, size);

			if(tail_size >0){
				chunk_list_insert(&freed_chunks, chunk.start + size, tail_size);
			}
			return chunk.start;
		}
	}
	return NULL;
}

/*
It copy the data in  the src to dest 
*/
void chunk_cpy(void *dest, void *src,size_t size){
	char *d = (char *)dest;
	char *s = (char *)src;

	for(size_t i =0;i<size; i++){
		d[i] = s[i];
	}
}

/*
It find the index of the pointer using chunk_list_find func
and then insert the chunk in the freed_chunks list and remove that chunk from the the alloced_chunks list

*/

void heap_free(void *ptr){
	if(ptr == NULL) return;

	const int index = chunk_list_find(&alloced_chunks, ptr);
	printf("index %d",index);
	assert(index >= 0);
	chunk_list_insert(&freed_chunks, alloced_chunks.chunks[index].start, alloced_chunks.chunks[index].size);
	chunk_list_remove(&alloced_chunks, (size_t)index);
	 
}


/*
This function realloc memory. It doesn't resize it instead of that it gives a new pointer to a new chunk with the requested size.
and it copy all the data in the old chunk to the new chunk
and free the old chunk.

You can implement the resizing the chunk by just  checking the consecutive chunk of the additional space is is allocated or not.
if allocated replace with new chunk
else resize it

*/
void *heap_realloc(void *ptr, size_t size){
	if(size == 0) return NULL;
	if(ptr == NULL) return heap_alloc(size);
	
	int index = chunk_list_find(&alloced_chunks, ptr);
	if(index <0) return NULL;

	Chunk old_chunk = alloced_chunks.chunks[index];
	if(old_chunk.size >= size) UNIMPLEMENTED;
	
	void *new_ptr = heap_alloc(size);
	if(new_ptr == NULL) return NULL;
	
	chunk_cpy(new_ptr, ptr, old_chunk.size);

	heap_free(ptr);
	return new_ptr;
		

}


/*garbage collector */

void heap_colle(){

	UNIMPLEMENTED; 
}

	


