void* Mymalloc(size_t len) {
  
  void* addr = mmap(0,len + sizeof(size_t),PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,-1,0);
  *(size_t*)addr = len;	
  return addr + sizeof(size_t);
  
}

