#include "SDL_system.h"
#include "SDL_mutex.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

char new_path[1024];

unsigned char **alloc2D(int row, int col)
{

	unsigned char ** ptr = (unsigned char **) malloc(sizeof(unsigned char *)*row);
	//memset(ptr, 0, sizeof(sizeof(unsigned char *)*row)); 
	int size = sizeof(unsigned char)*col;
	for(int i = 0; i < row; i++)
	{
		ptr[i] = (unsigned char *) malloc(size);
		//memset(ptr[i], 0, sizeof(unsigned char)*col); 
	}
	return ptr;

/*
	unsigned char* (*arr)[col] = malloc(sizeof *arr * row);
	return arr;	
*/	
/*
	unsigned char ** ptr = (unsigned char **) malloc(sizeof(unsigned char *)*row + sizeof(unsigned char)*col*row);
	for(int i = 0; i < row; i++)
	{
		ptr[i] = (unsigned char *) (ptr[0] + i*sizeof(unsigned char)*col + sizeof(unsigned char *)*row);
	}
	return ptr;
	*/
}

void free2D(unsigned char ** ptr, int row, int col)
{
	
	for(int i = 0; i < row; i++)
	{
		free(ptr[i]);
	}
	free(ptr);

}

void Check(const char *str)
{
#if 1	
	heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
	//heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
	printf("%s - \n", str);	
	heap_caps_check_integrity_all(true);  
	
	SDL_Delay(2000);    
	printf("OK\n");	
#endif	
}

struct SDL_mutex
{
    pthread_mutex_t id;
#if FAKE_RECURSIVE_MUTEX
    int recursive;
    pthread_t owner;
#endif
};

void SDL_Delay(Uint32 ms)
{
	//printf("Delay %d ", ms);
    const TickType_t xDelay = ms / portTICK_PERIOD_MS;
    vTaskDelay( xDelay );
}

IRAM_ATTR Uint32 SDL_GetTicks(void)
{
    return esp_timer_get_time() / 1000;    
}

/*
char *SDL_GetError(void)
{
    return (char *)"";
}
*/
bool initVideo = false;

int SDL_Init(Uint32 flags)
{
    if(flags == SDL_INIT_VIDEO && !initVideo)
	{
        SDL_InitSubSystem(flags);
		initVideo = true;
	}
    return 0;
}

void SDL_Quit(void)
{

}

void SDL_InitSD(void)
{
    printf("Initialising SD Card\n");
#if 1
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.command_timeout_ms = 3000;
    host.max_freq_khz = SDMMC_FREQ_DEFAULT; //SDMMC_FREQ_PROBING; //SDMMC_FREQ_DEFAULT;
    // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/spi_master.html
    host.slot = /*CONFIG_HW_SD_PIN_NUM_MISO == 19 ? VSPI_HOST :*/ HSPI_HOST;
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = CONFIG_HW_SD_PIN_NUM_MISO;
    slot_config.gpio_mosi = CONFIG_HW_SD_PIN_NUM_MOSI;
    slot_config.gpio_sck  = CONFIG_HW_SD_PIN_NUM_CLK;
    slot_config.gpio_cs   = CONFIG_HW_SD_PIN_NUM_CS;
	slot_config.dma_channel = 1; //2

#else
	sdmmc_host_t host = SDMMC_HOST_DEFAULT();
	host.flags = SDMMC_HOST_FLAG_1BIT;
	//host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
	host.command_timeout_ms=1500;
	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
	slot_config.width = 1;
#endif
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 15
    };

	sdmmc_card_t* card;
    SDL_LockDisplay();
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount("/sd", &host, &slot_config, &mount_config, &card));
    SDL_UnlockDisplay();

    printf("Init_SD: SD card opened.\n");
    
	//sdmmc_card_print_info(stdout, card);    
}

const SDL_version* SDL_Linked_Version()
{
    SDL_version *vers = malloc(sizeof(SDL_version));
    vers->major = SDL_MAJOR_VERSION;                 
    vers->minor = SDL_MINOR_VERSION;                 
    vers->patch = SDL_PATCHLEVEL;      
    return vers;
}

void SDL_DestroyMutex(SDL_mutex* mutex)
{

}

SDL_mutex* SDL_CreateMutex(void)
{
    SDL_mutex* mut = NULL;
    return mut;
}

int SDL_LockMutex(SDL_mutex* mutex)
{
    return 0;
}

int SDL_UnlockMutex(SDL_mutex* mutex)
{
    return 0;
}


int __mkdir(const char *path, mode_t mode)
{
	SDL_LockDisplay(); 
    int out = mkdir(path, mode);   
    SDL_UnlockDisplay();
    return out;
}

void update_path(const char *path)
{
	if(path[0] != '/')
	{
		strcpy(new_path, "/sd/data/keen/bin/");
		strcat(new_path, path);
		//sprintf(new_path, "/sd/data/keen/bin/%s", path);
	} else {
		strcpy(new_path, path);
	}

	//printf("%s\n", new_path);
}

FILE *__fopen( const char *path, const char *mode )
{
	update_path(path);
	
	SDL_LockDisplay();
	FILE *f = fopen(new_path, mode);
	SDL_UnlockDisplay();
	return f;
}

long __ftell( FILE *f )
{
	SDL_LockDisplay();
	long size = ftell(f);
	SDL_UnlockDisplay();
	return size;
}

int __feof ( FILE * stream )
{
	SDL_LockDisplay();
	int ret = feof ( stream );
	SDL_UnlockDisplay();
	return ret;	
}

int __fputc ( int character, FILE * stream )
{
	SDL_LockDisplay();
	int ret = fputc ( character, stream );
	SDL_UnlockDisplay();
	return ret;	
}

int __fgetc ( FILE * stream )
{
	SDL_LockDisplay();
	int ret = fgetc ( stream );
	SDL_UnlockDisplay();
	return ret;	
}

size_t __fwrite ( const void * ptr, size_t size, size_t count, FILE * stream )
{
	SDL_LockDisplay();
	size_t ret = fwrite ( ptr, size, count, stream );
	SDL_UnlockDisplay();
	return ret;		
}

int __fclose ( FILE * stream )
{
	SDL_LockDisplay();
	int ret = fclose ( stream );
	SDL_UnlockDisplay();
	return ret;	
}

int __fseek( FILE * stream, long int offset, int origin )
{
	SDL_LockDisplay();
	int ret = fseek ( stream, offset, origin );
	SDL_UnlockDisplay();
	return ret;
}

size_t __fread( void *buffer, size_t size, size_t num, FILE *stream )
{
	SDL_LockDisplay();
	size_t num_read = fread(buffer, size, num, stream);
	SDL_UnlockDisplay();
	return num_read;
}

int __stat(const char *path, struct stat *buf)
{
	SDL_LockDisplay();
	int ret = stat ( path, buf );
	SDL_UnlockDisplay();
	return ret;	
}

int __open(const char *path, int oflag, ...)
{
	update_path(path);

	SDL_LockDisplay();
	int ret = open(new_path, oflag);
	SDL_UnlockDisplay();
	return ret;	
}

int __close(int fildes)
{
	SDL_LockDisplay();
	int ret = close(fildes);
	SDL_UnlockDisplay();
	return ret;	
}

ssize_t __read(int fildes, void *buf, size_t nbyte)
{
	SDL_LockDisplay();
	ssize_t ret = read(fildes, buf, nbyte);
	SDL_UnlockDisplay();
	return ret;	
}

ssize_t __write(int fildes, const void *buf, size_t nbyte)
{
	SDL_LockDisplay();
	ssize_t ret = write(fildes, buf, nbyte);
	SDL_UnlockDisplay();
	return ret;	
}

off_t __lseek(int fd, off_t offset, int whence)
{
	SDL_LockDisplay();
	off_t ret = lseek(fd, offset, whence);
	SDL_UnlockDisplay();
	return ret;	
}

int __unlink(const char *pathname)
{

	SDL_LockDisplay();
	int ret = unlink(pathname);
	SDL_UnlockDisplay();
	return ret;		
}

DIR *__opendir(const char *name)
{
	update_path(name);

	SDL_LockDisplay();
	DIR *ret = opendir(new_path);
	SDL_UnlockDisplay();
	return ret;		
}

struct dirent *__readdir(DIR *dirp)
{
	SDL_LockDisplay();
	struct dirent *ret = readdir(dirp);
	SDL_UnlockDisplay();
	return ret;		
}

int __closedir(DIR *dirp)
{
	SDL_LockDisplay();
	int ret = closedir(dirp);
	SDL_UnlockDisplay();
	return ret;	
}

/*
DIR *__fdopendir(int fd)
{
	SDL_LockDisplay();
	DIR *ret = fdopendir(fd);
	SDL_UnlockDisplay();
	return ret;		
}
*/