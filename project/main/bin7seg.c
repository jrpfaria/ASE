char* getWeatherState(int Z)
{
    switch (Z)
    {                             /*____*/
        case 1: case 10: case 20:/*     */
        case 2: case 11: case 21:/*  S  */ 
        case 3: case 4: case 12:/*   A  */
        case 13: case 14: case 23:/* D  */
        case 24:return"SUNNY--";/*   D  */
        case 5: case 6: case 7:/*    A  */
        case 8: case 9: case 15:/*   M  */
        case 16: case 17: case 18:/* H  */
        return "RAINY--";         /* U  */
        case 19: case 31: case 32:/* S  */
        return "GALE--";          /* S  */
        case 22: case 25: case 26:/* E  */
        case 27: case 28: case 29:/* I  */
        case 30:return"CLOUDY--";/*  N  */
        default:return"EARLY--";/*______*/
    }
}

int char2seg(char c)
{
    switch (c)
    {
        case 'A': return 0b1110111;
        case 'C': return 0b0111001;
        case 'D': return 0b1001111;
        case 'E': return 0b1111001;
        case 'G': return 0b1111101;
        case 'I': return 0b0110000;
        case 'L': return 0b0111000;
        case 'N': return 0b1010100;
        case 'O': return 0b1011100;
        case 'R': return 0b1010000;
        case 'S': return 0b1101101;
        case 'U': return 0b0111110;
        case 'Y': return 0b1101110;
        case '-': return 0b1000000;
    
        default: return 0;
    }
}