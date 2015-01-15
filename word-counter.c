#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char* checkPunctuation(char* word);
int searchArray(char** dictionary, int wordCount, char* word);
int insertWord(char** dictionary, int* countingArray, int wordCount, char* word);
void printDictionary(char** dictionary, int* countingArray, int wordCount);
void extendArraySize(char*** dictionary, int**countingArray, int wordCount, int newSize);
void deleteDictionary(char** dictionary, int* countingArray, int wordCount);
void saveDictionary(char** dictionary, int* countingArray, char* saveFile, int wordCount);

int main(int argc, char *argv[]){
  //check for correct input
  if(argc != 3){
    printf("Incorrect number of inputs. Exiting Now...\n");
    exit(0);
  }
  FILE *readfp = fopen(argv[1], "r");
  if(readfp == NULL){
    printf("Error opening file to be read\n");
    exit(0);
  }
  
  /*Build initial dictionary with capacity for 64 words*/
  int dictionarySize = 64;
  char** dictionary = (char **)malloc(sizeof(char *)*dictionarySize);
  /*Build array to count each word*/
  int* countingArray = (int *)malloc(sizeof(int)*dictionarySize);

  int wordCount = 0; /*The number of words in the dictionary*/
  int rawWordCount = 0; /*The number of words read from file*/
  char tempWord[64];
  while(fscanf(readfp, "%s", tempWord) == 1){
    char* revised = checkPunctuation(tempWord);
    rawWordCount++;
    /*Convert to lower case*/
    for(int i = 0; i < strlen(revised); i++){
      revised[i] = tolower(revised[i]);
    }
    /*check if word is in array*/
    int wordCheck = searchArray(dictionary, wordCount, revised);
    if(wordCheck < 0){
      /*What happens when word is not found*/
      if(wordCount == dictionarySize){
	//Expand dictionary and counting array
	dictionarySize = dictionarySize*2;
	extendArraySize(&dictionary, &countingArray, wordCount, dictionarySize);
      }
      insertWord(dictionary, countingArray, wordCount, revised);
      wordCount++;
    } else {
      countingArray[wordCheck]++;
    }
  }
  printDictionary(dictionary, countingArray, wordCount);
  saveDictionary(dictionary, countingArray, argv[2], wordCount);
  deleteDictionary(dictionary, countingArray, wordCount);
}

char* checkPunctuation(char* word){
  int length = strlen(word);
  char punctuationMarks[17] = {',', '.', ';', '!', '\"', '(', ')', '?', '/', '[', ']', ':', '$', '<', '>', '#', '&'};
  char *revisedWord = (char *)malloc(sizeof(char)*(length + 1));
  int revisedWordLength = 0;
  for(int i = 0; i < length; i++){
    int puncFlag = 0;
    int possessive = 0;
    for(int j = 0; j < 16; j++){
      if(word[i] == punctuationMarks[j]){
	puncFlag = 1;
	break;
      } else if(word[i] == '\''){
	/*Take care of the possessive case*/
	puncFlag = 1;
	if((i < length - 1) && word[i + 1] == 's'){
	  possessive = 1;
	}
      }
    }
    if(puncFlag == 0){
      revisedWord[revisedWordLength] = word[i];
      revisedWordLength++;
    } else if(puncFlag == 1 && possessive == 1){
      /*For the possessive case*/
      i++;
    }
  }
  revisedWord[revisedWordLength] = '\0';
  return revisedWord;
}

/*Returns index if word is found in dictionary. -1 if not found*/
int searchArray(char** dictionary, int wordCount, char* word){
  int found = -1;
  for(int i = 0; i < wordCount; i++){
    if(strcmp(dictionary[i], word) == 0){
      found = i;
    }
  }
  return found;
}

/*Inserts word into correct alphabetical location in dictionary. Returns index at which it was inserted*/
int insertWord(char** dictionary, int* countingArray, int wordCount, char* word){
  if(wordCount == 0){
    /*Case for empty dictionary*/
    dictionary[0] = word;
    countingArray[0] = 1;
    return 0;
  } else if(wordCount == 1){
    if(strcmp(dictionary[0], word) < 0){
      /*Word goes after first entry*/
      dictionary[1] = word;
      countingArray[1] = 1;
      return 1;
    } else {
      /*Word goes before first entry*/
      dictionary[1] = dictionary[0];
      countingArray[1] = countingArray[0];
      dictionary[0] = word;
      countingArray[0] = 1;
      return 0;
    }
  } else {
    /*For dictionaries with more than 2 words*/
    /*Assumes there is enough room*/
    for(int i = 0; i < wordCount; i++){
      int comparison = strcmp(dictionary[i], word);
      if(comparison > 0){
	char* tempWord = dictionary[i];
	int tempCount = countingArray[i];
	dictionary[i] = word;
	countingArray[i] = 1;
	for(int j = i + 1; j <= wordCount; j++){
	  char* tempWord2 = dictionary[j];
	  int tempCount2 = countingArray[j];
	  dictionary[j] = tempWord;
	  countingArray[j] = tempCount;
	  tempWord = tempWord2;
	  tempCount = tempCount2;
	}
	return i;
      }
    }
    /*Case that word is simply appended to the end*/
    dictionary[wordCount] = word;
    countingArray[wordCount] = 1;
    return wordCount;
  }
}

void printDictionary(char** dictionary, int* countingArray, int wordCount){
  for(int i = 0; i < wordCount; i++){
    printf("The word %s appears %d times\n", dictionary[i], countingArray[i]);
  }
}

void extendArraySize(char*** dictionary, int**countingArray, int wordCount, int newSize){
  //Malloc new array
  //Copy everything over
  //Delete old array
  char** oldDPointer = *dictionary;
  char** newDPointer = (char**)malloc(sizeof(char *)*(newSize));
  int* oldCountArray = *countingArray;
  int* newCountArray = (int *)malloc(sizeof(int)*newSize);
  for(int i = 0; i < wordCount; i++){
    newDPointer[i] = (*dictionary)[i];
    newCountArray[i] = (*countingArray)[i];
  }
  *dictionary = newDPointer;
  *countingArray = newCountArray;
  
  free(oldDPointer);
  free(oldCountArray);
}

void deleteDictionary(char** dictionary, int* countingArray, int wordCount){
  for(int i = 0; i < wordCount; i++){
    free(dictionary[i]);
  }
  free(dictionary);
  free(countingArray);
}

void saveDictionary(char** dictionary, int* countingArray, char* saveFile, int wordCount){
  FILE* savefp = fopen(saveFile, "w+");
  for(int i = 0; i < wordCount; i++){
    fprintf(savefp, "%s: %d\n", dictionary[i], countingArray[i]);
  }
}
