#ifndef SIM_CACHE_H
#define SIM_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <inttypes.h>
#include <math.h>
#include <set>
#include <iomanip>

using namespace std;
typedef 
struct {
   uint32_t BLOCKSIZE;
   uint32_t L1_SIZE;
   uint32_t L1_ASSOC;
   uint32_t L2_SIZE;
   uint32_t L2_ASSOC;
   uint32_t PREF_N;
   uint32_t PREF_M;
} cache_params_t;

// Put additional data structures here as per your requirement.

class cache
{
   private:
   cache* L2_flag = NULL;
   uint32_t **cache_contents;
   uint32_t **cache_addr;
   int **cache_valid;
   int **cache_dirty;
   int **cache_LRU;
   uint32_t **prefetch_contents;
    int *prefetch_LRU;
   public:
   int reads = 0;
    int read_miss = 0;
    int write = 0;
    int write_miss = 0;
    int write_back = 0;
    int prefetch_count=0;
   uint32_t cache_assoc;
   uint32_t cache_set;  
   uint32_t offset_bits;
   uint32_t index_bits;
   uint32_t tag_bits;
   bool prefetch_enable=false;
    int prefetch_M;
    int prefetch_N;
   cache(uint32_t size, uint32_t assoc, uint32_t block_size,bool prefetch_flag, uint32_t prefetch_n, uint32_t prefetch_m) 
   {    
        if(size > 0 && block_size > 0 && assoc > 0) //Initialize cache
        {
            cache_assoc = assoc;
            cache_set = size / (block_size * assoc);
            offset_bits = log2(block_size);
            index_bits = log2(cache_set);
            tag_bits = 32-offset_bits-index_bits;
            int LRU_counter_cache;
            cache_contents = new uint32_t*[cache_set];
            cache_addr = new uint32_t*[cache_set];
            cache_valid = new int*[cache_set];
            cache_dirty = new int*[cache_set];
            cache_LRU = new int*[cache_set];
            for(int i = 0; i < cache_set; i++)
            {
                cache_contents[i] = new uint32_t[cache_assoc]; 
                cache_addr[i] = new uint32_t[cache_assoc]; 
                cache_valid[i] = new int[cache_assoc];
                cache_dirty[i] = new int[cache_assoc];
                cache_LRU[i] = new int[cache_assoc];
            }
            for (int i = 0; i < cache_set; i++) 
            {   
                //LRU_counter_cache = 0;
                for (int j = 0; j < assoc; j++)
                {
                    cache_contents[i][j] = 0;
                    cache_addr[i][j] = 0;
                    cache_LRU[i][j] = 0;
                    cache_valid[i][j] = 0;
                    cache_dirty[i][j] = 0;
                    //LRU_counter_cache++;
                }
            }
         }
         if(prefetch_flag)//Initialize prefetch
        {
            //int count=0;
            if(prefetch_m > 0 && prefetch_n > 0)
            {
                prefetch_enable = prefetch_flag;
                prefetch_M = prefetch_m;
                prefetch_N = prefetch_n;
                prefetch_contents = new uint32_t*[prefetch_N];
                prefetch_LRU = new int[prefetch_N];
                for(int i = 0; i < prefetch_N; i++)
                {
                    prefetch_contents[i] = new uint32_t[prefetch_M];
                }
                for(int i = 0; i < prefetch_N; i++)
                {
                    prefetch_LRU[i]=0;//count;
                    for(int j = 0; j < prefetch_M; j++)
                    {
                        prefetch_contents[i][j]=0;
                    }
                    //count++;
                }
            }
        }
      }
   void set_cache_content(uint32_t input_tag, int index, int way)
    {  
        cache_contents[index][way]= input_tag; 
    }
    void set_cache_address(uint32_t input_addr, int index, int way)
    {  
        cache_addr[index][way]= input_addr; 
    }
    void set_cache_valid(int valid_bit, int index, int way)
    {  
        cache_valid[index][way]= valid_bit; 
    }
    void set_cache_dirty(int dirty_bit, int index, int way)
    {  
        cache_dirty[index][way]= dirty_bit; 
    }
    void set_cache_LRU(int LRU, int index, int way)
    {  
        cache_LRU[index][way]= LRU; 
    }
    void set_prefetch_content(uint32_t input_tag_index, int n, int m)
    {  
        prefetch_contents[n][m]= input_tag_index; 
    }
    void set_prefetch_LRU(int LRU, int n)
    {  
        prefetch_LRU[n]= LRU; 
    }
    uint32_t get_cache_content(int index, int way)
    {
        uint32_t temp;
        temp = cache_contents[index][way];
        return temp;
    }
    uint32_t get_cache_addr(int index, int way)
    {
        uint32_t temp_addr;
        temp_addr = cache_addr[index][way];
        return temp_addr;
    }
    int get_cache_LRU(int index, int way)
    {
        uint32_t temp_LRU;
        temp_LRU = cache_LRU[index][way];
        return temp_LRU;
    }
    int get_cache_valid(int index, int way)
    {
        int temp_valid;
        temp_valid = cache_valid[index][way];
        return temp_valid;
    }
    int get_cache_dirty(int index, int way) 
    {
        int temp_Dirty;
        temp_Dirty = cache_dirty[index][way];
        return temp_Dirty;
    }
    uint32_t get_prefetch_content(int n,int m)
    {
        uint32_t temp;
        temp = prefetch_contents[n][m];
        return temp;
    }
    int get_prefetch_LRU(int n)
    {
        uint32_t temp_LRU;
        temp_LRU = prefetch_LRU[n];
        return temp_LRU;
    }
    void setL2_flag(cache* x){ L2_flag = x; } //if not null cache level 2 exists
    cache* getL2_flag(){
        return L2_flag;
    } //get the L2 flag value
};
uint32_t tag_decoder(uint32_t addr, cache s){
    uint32_t tag_temp;
    uint32_t index_temp;
    uint32_t offSet;

    offSet = s.offset_bits;
    index_temp = s.index_bits;
    tag_temp = addr >> (offSet + index_temp);   //Offset + index + tag  = address. shift right

    return tag_temp;
}

uint32_t tag_index_decoder(uint32_t addr, cache s)
{
    uint32_t tag_temp;
    uint32_t offSet;

    offSet = s.offset_bits;
    tag_temp = addr >> offSet;   //Offset + index + tag  = address. shift right

    return tag_temp;
}

uint32_t index_decoder(uint32_t addr, cache s){
    uint32_t index_temp;
    uint32_t offSet;
    uint32_t index_value; //final index value
    int twos_power;

    offSet = s.offset_bits; //Number of offset bits
    index_temp = s.index_bits; //Number for index bits
    index_value =  addr >> offSet; //shift right
    twos_power = pow(2, index_temp) - 1;  //2^index - 1
    index_value = index_value & twos_power;
    
    return index_value;
}
//updates LRU of the cache
void LRU_tracker(uint32_t LRU_index, uint32_t LRU_tag , uint32_t block_index ,cache *LRU_cache){
    
    uint32_t most_recently_used = LRU_cache->cache_assoc-1; //total way - 1
    uint32_t get_LRU = LRU_cache->get_cache_LRU(LRU_index,block_index); //value at the block of hit LRU
    uint32_t current_get_LRU; //holds current interations of LRU
    
    for(uint32_t k = 0; k <= most_recently_used; k++){
        current_get_LRU = LRU_cache->get_cache_LRU(LRU_index,k);  //get current lru of each block in that set
        
        if(current_get_LRU > get_LRU) {  //if current LRU value is greater than the hit lru value decrement the LRU counter
            LRU_cache->set_cache_LRU(current_get_LRU-1,LRU_index,k);
        }
    }
    LRU_cache->set_cache_LRU(most_recently_used,LRU_index,block_index); //sets the blocks that it hits to max counter or MRU
}

//Check if the tag exists in the cache
bool check_exists( uint32_t tag, uint32_t index, cache *s_1) {
    for(uint32_t i = 0; i < s_1->cache_assoc; i++) {  
        if(s_1->get_cache_content(index,i) == tag) { //if the tag bits match then return true 
            return true; //does exist
        }
    }
    return false; //does not exist
}

//returns current block index
uint32_t LRU_index(uint32_t tag, uint32_t index, cache *s_1) {
    for(uint32_t i = 0; i < s_1->cache_assoc; i++) {  
        if(s_1->get_cache_content(index,i) == tag) { //if the tag bits match then return true
            return i; //returns block index
        }
    }
    return 0; //return null
}

//returns the LRU block index
uint32_t least_LRU_block(uint32_t store_tag_L2, uint32_t index, cache LRU_cache){
    uint32_t size_blocks = LRU_cache.cache_assoc; //number of blocks total
    
    for(uint32_t h = 0; h < size_blocks; h++) { //increment till the number of blocks
        if(LRU_cache.get_cache_LRU(index,h) == 0) {
            return h; //return the least LRU block
        }
    }
    return 0;
}
//Check for victim cache
bool check_eviction(uint32_t evic_tag, uint32_t evic_index, cache *s1) {
    uint32_t least_LRU_b;
    int block_getDirty;
    
    least_LRU_b = least_LRU_block(evic_tag, evic_index, *s1); //get the index of block with LRU
    block_getDirty = s1->get_cache_dirty(evic_index,least_LRU_b); //get dirty block bit

    if(block_getDirty) {
        return true;
    }
    return false;
}
//Check if tag+index exists in prefetch
bool check_hit_prefetch(uint32_t tag_index, cache *s1)
{
    for(int i = 0;i < s1->prefetch_N; i++)
    {
        for(int j = 0;j < s1->prefetch_M; j++)
        {
            if(s1->get_prefetch_content(i,j) == tag_index)
            {
                return true;
            }
        }
    }
    return false;
}
//Update the prefetch contents
void update_prefetch_contents(uint32_t tag_index,int n, cache *s1)
{
    int store_tag_index = tag_index + 1;
    for(int i = 0; i < s1->prefetch_M; i++)
    {
        s1->set_prefetch_content(store_tag_index,n,i);
        store_tag_index = store_tag_index + 1;
    }
}
//Update the LRU of the prefetch
void LRU_update_prefetch(uint32_t prefetch_number ,cache *s1){
    
    //printf("LRU-tracker used");
    uint32_t most_recently_used = s1->prefetch_N-1; //total #blocks - 1
    uint32_t get_LRU = s1->get_prefetch_LRU(prefetch_number); //value at the block of hit LRU
    uint32_t current_get_LRU; //holds current interations of LRU
    
    for(uint32_t k = 0; k <= most_recently_used; k++){
        current_get_LRU = s1->get_prefetch_LRU(k);  //get current lru of each block in that set
        
        if(current_get_LRU > get_LRU) {  //if current LRU value is greater than the hit lru value decrement the LRU counter
            s1->set_prefetch_LRU(current_get_LRU-1,k);
        }
    }
    s1->set_prefetch_LRU(most_recently_used,prefetch_number); //sets the blocks that it hits to max counter or MRU
}
//Return the current prefetch stream
int current_prefetch_number(uint32_t tag, cache *s1)
{
    for(int i =0; i < s1->prefetch_N;i++)
    {
        for(int j = 0; j < s1->prefetch_M;j++)
        if(s1->get_prefetch_content(i,j)==tag)
        {
            return i;
        }
    }
    return 0;

}
int current_prefetch_count(uint32_t tag, cache *s1)
{
    for(int i =0; i < s1->prefetch_N;i++)
    {
        for(int j = 0; j < s1->prefetch_M;j++)
        if(s1->get_prefetch_content(i,j)==tag)
        {
            return j;
        }
    }
    return 0;

}
//Return the least used prefetch stream
int least_LRU_prefetch(cache *s1)
{
    for(int i = 0; i < s1->prefetch_N;i++)
    {
        if(s1->get_prefetch_LRU(i)==0)
        {
            return i;
        }
    }
    return 0;
}

void read_write_process(char rw, uint32_t addr, cache *s1) {
    uint32_t store_tag;
    uint32_t store_tag_index;
    uint32_t store_index;
    uint32_t least_LRU_b;
    uint32_t eviction_block_addr;
    uint32_t current_index;
    uint32_t update_LRU_index;
    char read = 'r';
    char write ='w';
    int dirty_L = 0;
    int dirty_H = 1;
    int valid = 1;
    
    store_tag   = tag_decoder(addr, *s1);  //decode tag
    store_index = index_decoder(addr, *s1); //decode index
    store_tag_index = tag_index_decoder(addr, *s1);
    least_LRU_b = least_LRU_block(store_tag, store_index, *s1);
    bool cache_exists=check_exists(store_tag, store_index, s1);

    if(rw == 'r') { s1->reads++; } else { s1->write++; } //check if read or write then increment counter
    
    if(s1->prefetch_enable)
    {
        int count=s1->prefetch_count;
        int update_prefetch;
        int least_prefetch;
        int prefetch_count_M=0;
        bool prefetch_exists = check_hit_prefetch(store_tag_index,s1);
        if(prefetch_exists)
        {
            if(cache_exists)
            {
                prefetch_count_M=current_prefetch_count(store_tag_index,s1)+1;
                update_prefetch = current_prefetch_number(store_tag_index,s1);
                update_prefetch_contents(store_tag_index,update_prefetch,s1);
                LRU_update_prefetch(update_prefetch,s1);
                count = count + prefetch_count_M;
            }
            else
            {
                if(rw == 'r')
                {
                    s1->read_miss--;
                }
                else
                {
                    s1->write_miss--;
                }
                prefetch_count_M=current_prefetch_count(store_tag_index,s1)+1;
                update_prefetch = current_prefetch_number(store_tag_index,s1);
                update_prefetch_contents(store_tag_index,update_prefetch,s1);
                LRU_update_prefetch(update_prefetch,s1);
                count = count + prefetch_count_M;
            }
            s1->prefetch_count=count;
        }
        else
        {
            if(cache_exists)
            {
                count = count + 0;
            }
            else
            {
                least_prefetch=least_LRU_prefetch(s1);
                update_prefetch_contents(store_tag_index,least_prefetch,s1);
                update_prefetch = current_prefetch_number(store_tag_index,s1);
                LRU_update_prefetch(update_prefetch,s1);
                count = count + s1->prefetch_M;
            }
            s1->prefetch_count=count;
        }
    }

    if(cache_exists) 
    {
        //cout << "L1 hit" << endl;
        update_LRU_index = LRU_index(store_tag, store_index, s1);
        LRU_tracker(store_index, store_tag ,update_LRU_index,s1);
        current_index = LRU_index(store_tag,store_index, s1);
        if(rw == 'w') 
        { s1->set_cache_dirty(dirty_H,store_index,current_index);
           //cout << "L1 set dirty" << endl;
        } //set dirty bit high
        //cout << "L1 update LRU" << endl;
    }
    else 
    { //tag does not exist
        //cout << "L1 miss" <<endl;
        /*if(prefetch_exists)
        {
            if(rw == 'r') {
            
            s1->set_cache_content(store_tag,store_index,least_LRU_b); //bit set to clean
            s1->set_cache_address(addr,store_index,least_LRU_b);
            s1->set_cache_valid(valid,store_index,least_LRU_b);
            s1->set_cache_dirty(dirty_L,store_index,least_LRU_b);   
        }
        
        if(rw == 'w') {
           
            s1->set_cache_content(store_tag,store_index,least_LRU_b); //bit set to clean
            s1->set_cache_address(addr,store_index,least_LRU_b);
            s1->set_cache_valid(valid,store_index,least_LRU_b);
            s1->set_cache_dirty(dirty_H,store_index,least_LRU_b);
        }
        }
        else
        {*/
        if(check_eviction(store_tag, store_index, s1)) { //check for dirty
            eviction_block_addr = s1->get_cache_addr(store_index,least_LRU_b); //evicted block address
            //tag = tag_decoder(eviction_block_addr, block_s, *s1);
            //index = index_decoder(eviction_block_addr, block_s, *s1);
            s1->write_back++; //increment write back;
            //cout << "victim: "<< hex <<eviction_block_addr << " (tag " << tag << ", index" << dec << index << ", dirty)"<< endl;
            if(s1->getL2_flag()) { //call write to next level
                read_write_process(write, eviction_block_addr, s1->getL2_flag());
            }
        } //else { cout << "victim: none" << endl; }
        
        if(s1->getL2_flag()) { read_write_process(read, addr, s1->getL2_flag()); } //if next level exists
        if(rw == 'r') {
            s1->read_miss++;
            //cout << "L1 miss" <<endl;
            s1->set_cache_content(store_tag,store_index,least_LRU_b); //bit set to clean
            s1->set_cache_address(addr,store_index,least_LRU_b);
            s1->set_cache_valid(valid,store_index,least_LRU_b);
            s1->set_cache_dirty(dirty_L,store_index,least_LRU_b);   
        }
        
        if(rw == 'w') {
            s1->write_miss++;
            //cout << "L1 miss" <<endl;
            s1->set_cache_content(store_tag,store_index,least_LRU_b); //bit set to clean
            s1->set_cache_address(addr,store_index,least_LRU_b);
            s1->set_cache_valid(valid,store_index,least_LRU_b);
            s1->set_cache_dirty(dirty_H,store_index,least_LRU_b);
        }
        update_LRU_index = LRU_index(store_tag, store_index, s1);
        LRU_tracker(store_index, store_tag, update_LRU_index,s1);
        //}
    }
}

void print_sim_results(cache *s1, cache *s2) 
{
    int reads = s1->reads;
    int write = s1->write;
    int read_miss = s1->read_miss;
    int write_miss = s1->write_miss;
    int write_back = s1->write_back;
    double L1_miss_rate = (double)(read_miss + write_miss)/(reads + write);
    int L2_reads = 0;
    int L2_writes = 0;
    int L2_read_miss = 0;
    int L2_write_miss = 0;
    int L2_write_back = 0;
    double L2_miss_rate = 0.00;
    int L1_prefetch = s1->prefetch_count;
    int L2_prefetch = 0;
    int memory_traffic = read_miss + write_miss + write_back + L1_prefetch;
    int L2_read_L1_prefetch = 0;
    int L2_read_miss_L1_prefetch = 0;

    if(s1->getL2_flag())
    {
        L2_reads = s2->reads;
        L2_writes = s2->write;
        L2_read_miss = s2->read_miss;
        L2_write_miss = s2->write_miss;
        L2_write_back = s2->write_back;
        L2_miss_rate = (double) (L2_read_miss)/(L2_reads);
        L2_prefetch = s2->prefetch_count;
        memory_traffic = L2_read_miss + L2_write_miss + L2_write_back + s2->prefetch_count;
    }
    
    printf("===== Measurements =====\n");
    cout<<"a. L1 reads:                      "<<dec<<reads<<endl;
    cout<<"b. L1 read misses:                "<<dec<<read_miss<<endl;
    cout<<"c. L1 writes:                     "<<dec<<write<<endl;
    cout<<"d. L1 write misses:               "<<dec<<write_miss<<endl;
    cout<<"e. L1 miss rate:                  "<<fixed<<setprecision(4)<<L1_miss_rate<<endl;
    cout<<"f. L1 writebacks:                 "<<dec<<write_back<<endl;
    cout<<"g. L1 prefetches:                 "<<dec<<L1_prefetch<<endl;
    cout<<"h. L2 reads (demand):             "<<dec<<L2_reads<<endl;
    cout<<"i. L2 read misses (demand):       "<<dec<<L2_read_miss<<endl;
    cout<<"j. L2 reads (prefetch):           "<<dec<<L2_read_L1_prefetch<<endl;
    cout<<"k. L2 read misses (prefetch):     "<<dec<<L2_read_miss_L1_prefetch<<endl;
    cout<<"l. L2 writes:                     "<<dec<<L2_writes<<endl;
    cout<<"m. L2 write misses:               "<<dec<<L2_write_miss<<endl;
    cout<<"n. L2 miss rate:                  "<<fixed<<setprecision(4)<<L2_miss_rate<<endl;
    cout<<"o. L2 writebacks:                 "<<dec<<L2_write_back<<endl;
    cout<<"p. L2 prefetches:                 "<<dec<<L2_prefetch<<endl;
    cout<<"q. memory traffic:                "<<dec<<memory_traffic<<endl;
}

void print_cache(int cache_ind, cache *s1){
    
    uint32_t num_L1_sets = s1->cache_set;
    uint32_t num_blocks_L1 = s1->cache_assoc;
    uint32_t max_block_size = num_blocks_L1-1; //max_associativity
    uint32_t LRU_value = 0;
    uint32_t block_tag = 0;
    int dirty_bit = 0;
    uint32_t max_increment = max_block_size;
    uint32_t j = 0;
    
    if(cache_ind == 1) {
        printf("\n===== L1 contents =====\n");
    } else {
        printf("===== L2 contents =====\n");
    }
    
    
    for(uint32_t i = 0; i < num_L1_sets; i++) {
        max_block_size = max_increment;
        cout << "  set\t" << dec << i << ": " << "\t";
        for(uint32_t k = 0; k < num_blocks_L1; k++) { 
            for(j = 0; j < num_blocks_L1; j++) {
                LRU_value = s1->get_cache_LRU(i,j);
                block_tag = s1->get_cache_content(i,j);
                dirty_bit = s1->get_cache_dirty(i,j);
                    if(LRU_value == max_block_size) {
                        cout << hex << block_tag<<" ";
                        if(dirty_bit) { cout << " D"; }  else cout << "  ";
                        cout << "\t" ;
                        max_block_size--;
                        break;
                        }
                  }
        }
         printf("\n");
    }
    printf("\n");
}

void print_prefetch(cache *s)
{
    cout<<"===== Stream Buffer(s) contents =====\n";
    for(int i=0; i < s->prefetch_N; i++)
    {
           
        for(int k=0; k < s->prefetch_M; k++)
        {
            
            cout<<" "<<hex<<s->get_prefetch_content(i,k)<<" ";
            
        }
        cout<<"\n";
        
    
    }
    printf("\n");
}
#endif