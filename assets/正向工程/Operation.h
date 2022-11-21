/**
 * Project Untitled
 */


#ifndef _OPERATION_H
#define _OPERATION_H

class Operation {
public: 
    
void Operation();
    
/**
 * @param pass
 */
void Recording(int pass);
    
/**
 * @param window
 */
void MainMenu(HWND& window);
    
void ShowRecording();
    
/**
 * @param r
 * @param c
 */
void BlankOpen(int r, int c);
    
void boom();
    
int print();
};

#endif //_OPERATION_H