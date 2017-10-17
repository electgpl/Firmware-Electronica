char *itoa(long int num, char *s){     // Funcion ITOA (Entero to ASCII)
     unsigned long int temp=1;         // Declaracion de valor temporal
     unsigned int8 i, cnt=0;           // Declaracion de indices y contadores
     char c;                           // Declaracion de variable de caracter de salida
     while(temp>0){                    // Rutina de Conversion (Queda invertida)
        temp=(num/10);                 // Conversion de caracter a caracter
        s[cnt]=(num%10)+'0';           // utilizando divisiones y resto
        if(s[cnt]>0x39)                // sumando el offset de la tabla ASCII
           s[cnt]+=0x7;
        cnt++;
        num=temp;
     }
     for(i=0;i<(int8)(cnt/2);i++){     // Rutina para invertir el numero convertido
        c=s[i];                        // Intercambio de variables
        s[i]=s[cnt-i-1];
        s[cnt-i-1]=c;
     }
     s[cnt]='\0';                      // Caracter nulo, fin de la conversion ITOA
     return s;                         // Retorno del valor ASCII
}