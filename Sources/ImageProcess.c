
/*****************************************************
150 cm    0.8 cm/row
0  ~  4 : 70 cm (80~150) 88~0 (89)
5  ~  9 : 27 cm (53~80) 123~89 (35)
10 ~ 14 : 20 cm (33~53) 148~124 (25)
14 ~ 19 : 10 cm (23~33) 160~149 (12)
20 ~ 24 : 8 cm  (15~23) 170~161 (10)
25 ~ 29 : 6 cm  (9~15) 178~171 (8)
30 ~ 34 : 5 cm  (4~9) 184~179 (6)
35 ~ 39 : 4 cm  (0~4) 189~185 (5)
******************************************************/

#include "includes.h"

// Get threshold automatically
// Get the highest value of the smallest value in each edge for threshold
unsigned char GetThreshold(unsigned char x) // Get threshold of the nearest x rows
{
    unsigned char thre;
    void *nouse = NULL;
    unsigned char *ptr = NULL;
    int i, j;
    //unsigned int stat[16];
    unsigned int ftemp = 0;
    int iwhitepeak = 16, iblackpeak = 0;
    unsigned int sum = 0;
    Bool inc = 0, dec = 0;
    unsigned char inctime = 0, dectime = 0;
    nouse = memset( stat, 0, sizeof(stat) );
    thre = 0x00;
    for( i = ROW_VALUE - 1; i >= ROW_VALUE - x; i--)
    //for( i = 0; i < x; i++ )
    {
        ptr = &buff[i][0];
        for( j = 0; j <= COLUMN_VALUE - 1; j++ )
        {
            if( *ptr < 0x10 )
            {
                stat[0] += 1;
            }
            else if( *ptr >= 0x10 && *ptr < 0x20 )
            {
                stat[1] += 1;
            }
            else if( *ptr >= 0x20 && *ptr < 0x30 )
            {
                stat[2] += 1;
            }
            else if( *ptr >= 0x30 && *ptr < 0x40 )
            {
                stat[3] += 1;
            }
            else if( *ptr >= 0x40 && *ptr < 0x50 )
            {
                stat[4] += 1;
            }
            else if( *ptr >= 0x50 && *ptr < 0x60 )
            {
                stat[5] += 1;
            }
            else if( *ptr >= 0x60 && *ptr < 0x70 )
            {
                stat[6] += 1;
            }
            else if( *ptr >= 0x70 && *ptr < 0x80 )
            {
                stat[7] += 1;
            }
            else if( *ptr >= 0x80 && *ptr < 0x90 )
            {
                stat[8] += 1;
            }
            else if( *ptr >= 0x90 && *ptr < 0xA0 )
            {
                stat[9] += 1;
            }
            else if( *ptr >= 0xA0 && *ptr < 0xB0 )
            {
                stat[10] += 1;
            }
            else if( *ptr >= 0xB0 && *ptr < 0xC0 )
            {
                stat[11] += 1;
            }
            else if( *ptr >= 0xC0 && *ptr < 0xD0 )
            {
                stat[12] += 1;
            }
            else if( *ptr >= 0xD0 && *ptr < 0xE0 )
            {
                stat[13] += 1;
            }
            else if( *ptr >= 0xE0 && *ptr < 0xF0 )
            {
                stat[14] += 1;
            }
            else
            {
                stat[15] += 1;
            }
            ptr++;
        }
    }

    //process & get threshold

    for( i = 15; i >= 0; i-- ) //get the white peak position
    {
        if( stat[i] >= ftemp )
        {
            ftemp = stat[i];
            iwhitepeak = i;
        }
    }
    ftemp = stat[1];
    for( i = 1; i <= iwhitepeak - 5 ; i++ ) //get the black peak position
    {
        if( stat[i] > ftemp )
        {
            ftemp = stat[i];
            iblackpeak = i;
        }
    }
    ftemp = stat[iblackpeak];
    for( i = iblackpeak + 1; i < iwhitepeak; i++ )
    {
        if( stat[i] <= ftemp )
        {
            ftemp = stat[i];
        }
        else
        {
            thre = (i-1)*0x10;
            break;
        }
    }
    
    thre = (thre > THRESHOLDUPLIMIT) ? THRESHOLDUPLIMIT : thre;
    thre = (thre < THRESHOLDLIMIT) ? THRESHOLDLIMIT : thre;
    return thre;
}

int GetBlackPos(unsigned char RowToProcess) //Get black center(only process one row) RowToProcess:0~29
{
    int i = 0, j = 0;
    unsigned char *p = &buff[0][0];
    unsigned char *p_b = &data_b[0];
    int edge_r = -1, edge_l = -1;
    unsigned char varlimit = 0x40;
    unsigned char widthlimit = 20;
    unsigned char errlimit = 15; // if line center is beyond this limit, filter it!
    unsigned char search_start = COLUMN_VALUE/2;
    unsigned char detected = 0;    //Bool

    //find black
    if( (RowToProcess >= ROW_VALUE - 10) && (RowToProcess <= ROW_VALUE - 1) ) //between the row[39] and row[30]
    {
        ///===============  2010-07-01  ====================

        ///edge ditect

        if( continuous_valid_line < 2 )
        {
            //if( !flag_image_lost )
            if( 0 )
            {
                search_start = previous_nearest_linecenter;
                search_range = 50;
            }
            else
            {
                search_start = COLUMN_VALUE/2;
                search_range = 100;
            }
            
            search_l = ((search_start - search_range) < 4)
                        ? 4 : (search_start - search_range);
            search_r = ((search_start + search_range) > (COLUMN_VALUE - 5))
                        ? (COLUMN_VALUE - 5) : search_start + search_range;

            varlimit = 0x50;

            //filter for edge detect
            for( i = search_l, p = &buff[RowToProcess][search_l]; p < &buff[RowToProcess][search_r]; i++, p++ )
            {
                if( (*(p-1) - *p > 0x20) && (*(p+1) - *p > 0x20) )
                {
                    *p = (*(p+1) + *(p-1))/2;
                }
                if( (*p - *(p-1) > 0x20) && (*p - *(p+1) > 0x20) )
                {
                    *p = (*(p+1) + *(p-1))/2;
                }
            }

            // Detect edge from left to right
            for( i = search_l, p = &buff[RowToProcess][search_l]; p < &buff[RowToProcess][search_r]; i++, p++ )
            {
                if( *(p-4) - *(p+4) > varlimit )
                {
                    edge_l = i;
                    break;
                }
            }
            // Detect edge from right to left
            for( i = search_r, p = &buff[RowToProcess][search_r]; p > &buff[RowToProcess][search_l]; i--, p-- )
            {
                if( *(p+4) - *(p-4) > varlimit )
                {
                    edge_r = i;
                    break;
                }
            }
        }
        else
        {
            search_start = Line_Center[RowToProcess+1] + (Line_Center[RowToProcess+1] - Line_Center[RowToProcess+2]);
            search_start = search_start <= 0 ? 0 : search_start;
            search_start = search_start > COLUMN_VALUE - 1 ? COLUMN_VALUE - 1 : search_start;

            search_range = 30;

            search_l = ((search_start - search_range) < 4)
                        ? 4 : (search_start - search_range);
            search_r = ((search_start + search_range) > (COLUMN_VALUE - 5))
                        ? (COLUMN_VALUE - 5) : search_start + search_range;

            varlimit = 0x50;

            //filter for edge detect
            for( i = search_l, p = &buff[RowToProcess][search_l]; p < &buff[RowToProcess][search_r]; i++, p++ )
            {
                if( (*(p-1) - *p > 0x20) && (*(p+1) - *p > 0x20) )
                {
                    *p = (*(p+1) + *(p-1))/2;
                }
                if( (*p - *(p-1) > 0x20) && (*p - *(p+1) > 0x20) )
                {
                    *p = (*(p+1) + *(p-1))/2;
                }
            }

            // Detect edge from left to right
            for( i = search_l, p = &buff[RowToProcess][search_l]; p < &buff[RowToProcess][search_r]; i++, p++ )
            {
                if( *(p-4) - *(p+4) > varlimit )
                {
                    edge_l = i;
                    break;
                }
            }
            // Detect edge from right to left
            for( i = search_r, p = &buff[RowToProcess][search_r]; p > &buff[RowToProcess][search_l]; i--, p-- )
            {
                if( *(p+4) - *(p-4) > varlimit )
                {
                    edge_r = i;
                    break;
                }
            }
        }
        
        ///================================================

        if ( edge_l < edge_r && edge_l >= 0 && edge_r >= 0 && edge_r - edge_l <= 20 )
        {
            findroute = 1;
            continuous_valid_line += 1;

            blackposcenter = (edge_r + edge_l)/2;

            if( continuous_valid_line == 2 )
            {
                if( abs(blackposcenter - Line_Center[RowToProcess + 1]) > 50 )  //out of limit, that is, these 2 center is not adjacent
                {
                    continuous_valid_line = 1;
                }
                else   //do find the two continuous valid line, get position of the nearest valid line.
                {
                    nearest_valid_line = RowToProcess + 1;
                    flag_image_lost = 0;    //no problem
                    //complete neareast losted line centers
                    for( j = ROW_VALUE - 1; j > RowToProcess + 1; j-- )
                    {
                        Line_Center[j] = Line_Center[RowToProcess + 1];
                    }
                }
            }

        }
        else
        {
            findroute = 0;
            if( continuous_valid_line < 2 )
            {
                continuous_valid_line = 0;
                if( RowToProcess == ROW_VALUE - 10 )
                {
                    flag_image_lost = 1;    //if cannot find line in nearest 10 rows, image lost.
                }
            }
            get_lost_time += 1;
            if( RowToProcess == ROW_VALUE - 1 )
            {
                blackposcenter = previous_nearest_linecenter;
            }
            else
            {
                //blackposcenter = Line_Center[RowToProcess+1];
                blackposcenter = Line_Center[RowToProcess+1] + (Line_Center[RowToProcess+1] - Line_Center[RowToProcess+2]);
            }
        }

    }
    else
    {
        ///edge ditect
        if( RowToProcess < 10 )
        {
            search_range = 20;
        }
        else if( RowToProcess >= 11 && RowToProcess < 20 )
        {
            search_range = 18;
        }
        else
        {
            search_range = 15;
        }
        if( Line_Center[RowToProcess+1] == 0 || Line_Center[RowToProcess+1] == COLUMN_VALUE-1 )
        {
            search_range = 5;
        }

        if( findroute == 1 ) // if find route in previous search, this search depend on the line prediction
        {
            search_start = Line_Center[RowToProcess+1] + (Line_Center[RowToProcess+1] - Line_Center[RowToProcess+2]);
            search_start = search_start <= 0 ? 0 : search_start;
            search_start = search_start > COLUMN_VALUE - 1 ? COLUMN_VALUE - 1 : search_start;
        }
        else
        {
            search_start = Line_Center[RowToProcess+1];
        }

        search_l = ((search_start - search_range) < 4)
                    ? 2 : (search_start - search_range);
        search_r = ((search_start + search_range) > (COLUMN_VALUE - 5))
                    ? (COLUMN_VALUE - 5) : search_start + search_range;

        if( RowToProcess < 10 )
        {
            varlimit = 0x30;//30
            widthlimit = 10;
        }
        else if( RowToProcess >= 10 && RowToProcess < 20 )
        {
            varlimit = 0x40;//40
            widthlimit = 15;
        }
        else
        {
            varlimit = 0x50;//50
            widthlimit = 20;
        }

        //filter for edge detect
        for( i = search_l, p = &buff[RowToProcess][search_l]; p < &buff[RowToProcess][search_r]; i++, p++ )
        {
            if( (*(p-1) - *p > 0x20) && (*(p+1) - *p > 0x20) )
            {
                *p = (*(p+1) + *(p-1))/2;
            }
            if( (*p - *(p-1) > 0x20) && (*p - *(p+1) > 0x20) )
            {
                *p = (*(p+1) + *(p-1))/2;
            }
        }
        // Detect edge from left to right
        for( i = search_l, p = &buff[RowToProcess][search_l]; p < &buff[RowToProcess][search_r]; i++, p++ )
        {
            if( *(p-4) - *(p+4) > varlimit )
            {
                edge_l = i;
                break;
            }
        }
        // Detect edge from right to left
        for( i = search_r, p = &buff[RowToProcess][search_r]; p > &buff[RowToProcess][search_l]; i--, p-- )
        {

            if( *(p+4) - *(p-4) > varlimit )
            {
                edge_r = i;
                break;
            }
        }

        if ( edge_l < edge_r && edge_l > 0 && edge_r > 0 && edge_r - edge_l <= widthlimit )
        {
            findroute = 1;
            blackposcenter = (edge_r + edge_l)/2;
        }
        else
        {
            findroute = 0;
            get_lost_time += 1;
            // Predict line direction(IMPORTANT!!)
            if( RowToProcess <= 5 )
            {
                blackposcenter = Line_Center[RowToProcess+1];
            }
            else
            {
                blackposcenter = Line_Center[RowToProcess+1] + (Line_Center[RowToProcess+1] - Line_Center[RowToProcess+2]);
            }
        }
    }

    // limit two adjacent point err
    if( RowToProcess <= ROW_VALUE - 10 ) // modify error point when further than ROW_VALUE-1
    {
        //2010-05-10 new
        if( RowToProcess < 15 )
        {
            errlimit = 20;
        }
        else if( RowToProcess >= 15 && RowToProcess < 25 )
        {
            errlimit = 15;
        }
        else
        {
            errlimit = 10;
        }
        if( blackposcenter - Line_Center[RowToProcess+1] > errlimit || Line_Center[RowToProcess+1] - blackposcenter > errlimit  )
        {
            blackposcenter = Line_Center[RowToProcess+1];
        }
    }

    // move from above, in the bracket 2010-05-10
    if( blackposcenter < 0 )
    {
        blackposcenter = 0;
    }
    else if( blackposcenter > COLUMN_VALUE - 1 )
    {
        blackposcenter = COLUMN_VALUE - 1;
    }
    else;

    return blackposcenter;
}

void Check_start(void)
{
    unsigned char *p = &buff[0][0];
    unsigned char search_ll;
    unsigned char search_rr;
    int edge_ll[ROW_VALUE] = {0};
    int edge_rr[ROW_VALUE] = {0};
    unsigned char i,j;
    int ccc;
    int k;
    find_start = 0;
     
    for(i = ROW_VALUE-25;i<ROW_VALUE;i++)
    {
        for(p = &buff[i][1]; p < &buff[i][COLUMN_VALUE-2];p++ )
        {
            ///ÂË²¨
            if( (*(p-1) - *p > 0x20) && (*(p+1) - *p > 0x20) )
            {
                *p = (*(p+1) + *(p-1))/2;
            }
            if( (*p - *(p-1) > 0x20) && (*p - *(p+1) > 0x20) )
            {
                *p = (*(p+1) + *(p-1))/2;
            }
        }
        ///´Ó×óµ½ÓÒ¼ì±ßÑØ
        search_ll = ((Line_Center[i]-15) < 1)
                    ? 1 : (Line_Center[i]-15);
        search_rr = ((Line_Center[i]+15) > (COLUMN_VALUE - 2))
                    ? (COLUMN_VALUE - 2) : Line_Center[i]+15;
        for(j =search_ll,p = &buff[i][j]; p < &buff[i][Line_Center[i]-4];p++,j++ )
        {
            if( *(p+2) - *(p-2) > 0x40 )
            {
                edge_ll[i] = j;
                break;
            }
        }
        for(j =Line_Center[i]+4,p = &buff[i][j]; p < &buff[i][search_rr];p++,j++ )
        {
            if( *(p-2) - *(p+2) > 0x40 )
            {
                edge_rr[i] = j;
                break;
            }
        }
    }
    find_start = 0;
    for(i = ROW_VALUE-25;i<ROW_VALUE;i++)
    {
        if(edge_ll[i]!=0)
        {
            if(edge_rr[i-2]!=0||edge_rr[i-1]!=0||edge_rr[i]!=0||
               edge_rr[i+1]!=0||edge_rr[i+2]!=0)
            break;
        }
    }
//----------------------------------------------------
    if(i>0 && i<40)
    {
        search_l = ((Line_Center[i]-13) < 1)
                ? 1 : (Line_Center[i]-13);
        search_r = ((Line_Center[i]+13) > (COLUMN_VALUE - 2))
                ? (COLUMN_VALUE - 2) : (Line_Center[i]+13);
        ccc = 0;
        for(k = (i-1);k < (i+2);k++)
        {
            for( j = search_l, p = &buff[k][search_l]; p < &buff[k][search_r]; j++, p++ )
            {
                if( *(p-1) - *(p+1) > 0x26 )
                {
                    edge_ll[k] = j;
                    break;
                }
                else
                {
                    edge_ll[k] = 0;
                }
            }
    // Detect edge from right to left
            for( j = search_r, p = &buff[k][search_r]; p > &buff[k][search_l]; j--, p-- )
            {

                if( *(p+1) - *(p-1) > 0x26 )
                {
                    edge_rr[k] = j;
                    break;
                }
                else edge_rr[k] = 0;
            }

            if ( edge_ll[k] < edge_rr[k] && edge_ll[k] > 0 && edge_rr[k] > 0 && ((edge_rr[k] - edge_ll[k]) <= 15))
            {
                ccc++;
            }

        }
        if(ccc != 3)
        {
            i = 40;
        }
    }

//----------------------------------------------------
    find_start = i;    
    if( find_start > 0 && find_start < 40 )
    {
        flag_start = 1;
        BUZZER_ON();
            
        STOP();
        constant_speed = 0;
        isbreak = 1;
        controlspeed = 0;
    }
    else
    {
        BUZZER_OFF();
    }
}

void ImageProcess(void)
{
    int i;
    unsigned char counter = 0;
    Bool endflag = 0;
    float weight = 0.1;
    float blackposavgtemp = 0;
    unsigned int noblacktime = 0;
    int s_left = 0, s_right = 0, s_turn = 0;
    int s_turn_pos1 = 0, s_turn_pos2 = 0, s_width = 0;
    int cam_err = 0;
    int direction_err = 0;
    
    nearest_valid_line = -1;
    continuous_valid_line = 0;
    
    for( i = ROW_VALUE - 1; i >= 0; i-- )
    {
        Line_Center[i] = (int)GetBlackPos(i);
        
        //get the left and right peak position
        if( Line_Center[i] <= leftpeak )
        {
            leftpeak = Line_Center[i];
            leftpeakpos = i;
        }
        if( Line_Center[i] >= rightpeak )
        {
            rightpeak = Line_Center[i];
            rightpeakpos = i;
        }
        
        if( endflag == 0 )
        {
            if( findroute )
            {
                counter++;  // find the farest efficient black point(for speed control)
                noblacktime = 0;
            }
            else
            {
                noblacktime++;
                if( noblacktime >= 3 && i < ROW_VALUE - 10 )  // end while continuous no black >= 3
                {
                    endflag = 1;
                    noblacktime = 0;
                }
            }
        }
    }
    
    if( !flag_image_lost )
    {
        BUZZER_OFF();
        
        prediction = counter;
        
        for( i = ROW_VALUE - 1; i >= 0; i-- )
        {
            Line_Center_bak[i] = Line_Center[i];
        }
    }
    else
    {
        //BUZZER_ON();
        for( i = ROW_VALUE - 1; i >= 0; i-- )
        {
            Line_Center[i] = Line_Center_bak[i];
        }
    }
    
    line_dir = Line_Center[35] - Line_Center[39];
    
    nearest_linecenter = (Line_Center[35] + Line_Center[36] + Line_Center[37] + Line_Center[38] + Line_Center[39])/5;
    previous_nearest_linecenter = nearest_linecenter;
    
    ///far curve calculate
    farslope1 = farslope2 = nearslope1 = nearslope2 = 0;
    
    if( prediction > 3 )    //avoid 0 from being divided
    {
        farslope1 = (Line_Center[ROW_VALUE-prediction] - Line_Center[ROW_VALUE-prediction*2/3])*100/(prediction/3);
        farslope2 = (Line_Center[ROW_VALUE-prediction*2/3] - Line_Center[ROW_VALUE-1])*100/(prediction*2/3 - 1);
        farcurve = farslope1 - farslope2;
    }
    ///near curve calculate
    if( prediction >= 30 )
    {
        nearslope1 = (Line_Center[10] - Line_Center[16])*100/6;
        nearslope2 = (Line_Center[16] - Line_Center[39])*100/23;
    }
    else
    {
        nearslope1 = farslope1;
        nearslope2 = farslope2;
    }
    nearcurve = nearslope1 - nearslope2;

    slope_product_far = farslope1 * farslope2;
    slope_product_near = nearslope1 * nearslope2;
    
    //===========================================
    //route type judgement
    //NEED DEVELOPED!!
    
    routetype = 0;  //straight

    for( i = ROW_VALUE - 2; i >= 0; i-- )
    {
        if( Line_Center[i] < Line_Center[i+1] )
        {
            if( s_right > 0 )
            {
                if( s_right >= 3 )
                {
                    s_turn++;
                    if( s_turn == 1 )
                    {
                        s_turn_pos1 = i + 2;
                    }
                    else if( s_turn == 2 )
                    {
                        s_turn_pos2 = i + 2;
                    }
                    else;
                }
                s_right = 0;
            }
            s_left++;
        }
        else if( Line_Center[i] > Line_Center[i+1] )
        {
            if( s_left > 0 )
            {
                if( s_left >= 3 )
                {
                    s_turn++;
                    if( s_turn == 1 )
                    {
                        s_turn_pos1 = i + 2;
                    }
                    else if( s_turn == 2 )
                    {
                        s_turn_pos2 = i + 2;
                    }
                    else;
                }
                s_left = 0;
            }
            s_right++;
        }
        else;
    }

    s_width = abs( Line_Center[s_turn_pos1] - Line_Center[s_turn_pos2] );

    if( prediction >= 30 )
    {
        if( abs(farcurve) < 20 )
        {
            routetype = 0;  //straight
        }
        else
        {
            if( nearcurve < 20 )
            {
                routetype = 1;  //straight to turning
            }
            else
            {
                if( farcurve < 0 )
                {
                    routetype = 3;  //left
                }
                else
                {
                    routetype = 4;  //right
                }
            }
        }
    }
    else
    {
        if( abs(nearcurve) < 20 && Line_Center[ROW_VALUE - prediction] >= 30 && Line_Center[ROW_VALUE - prediction] <= 70 )
        {
            routetype = 5;  //slope or other problem
        }
        else
        {
            if( nearcurve < 0 )
            {
                routetype = 3;  //left
            }
            else
            {
                routetype = 4;  //right
            }
        }
    }
    
    if( s_turn >= 2 )
    {
        if( prediction > 30 && (Line_Center[s_turn_pos1] > 10 || Line_Center[s_turn_pos1] < 90) && (Line_Center[s_turn_pos2] > 10 || Line_Center[s_turn_pos2] < 90) )
        {
            if( s_width < 60 && s_width > 15 )
            {
                routetype = 2;
            }
        }
    }
    
    switch( routetype )
    {
        case 0:
            //counter_ss = counter_bs = counter_left = counter_right = counter_slope = 0;
            PORTA_PA0 = 0;
            PORTA_PA1 = 1;
            PORTA_PA2 = 1;
            PORTA_PA3 = 1;
            PORTA_PA4 = 1;
            PORTA_PA5 = 1;
            break;
        case 1:
            counter_straight = counter_bs = counter_left = counter_right = counter_slope = 0;
            PORTA_PA0 = 1;
            PORTA_PA1 = 0;
            PORTA_PA2 = 1;
            PORTA_PA3 = 1;
            PORTA_PA4 = 1;
            PORTA_PA5 = 1;
            break;
        case 2:
            counter_straight = counter_ss = counter_left = counter_right = counter_slope = 0;
            PORTA_PA0 = 1;
            PORTA_PA1 = 1;
            PORTA_PA2 = 0;
            PORTA_PA3 = 1;
            PORTA_PA4 = 1;
            PORTA_PA5 = 1;
            break;
        case 3:
            counter_left++;
            counter_straight = counter_ss = counter_bs = counter_right = counter_slope = 0;
            PORTA_PA0 = 1;
            PORTA_PA1 = 1;
            PORTA_PA2 = 1;
            PORTA_PA3 = 0;
            PORTA_PA4 = 1;
            PORTA_PA5 = 1;
            break;
        case 4:
            counter_right++;
            counter_straight = counter_ss = counter_bs = counter_left = counter_slope = 0;
            PORTA_PA0 = 1;
            PORTA_PA1 = 1;
            PORTA_PA2 = 1;
            PORTA_PA3 = 1;
            PORTA_PA4 = 0;
            PORTA_PA5 = 1;
            break;
        case 5:
            counter_straight = counter_ss = counter_bs = counter_left = counter_right = 0;
            PORTA_PA0 = 1;
            PORTA_PA1 = 1;
            PORTA_PA2 = 1;
            PORTA_PA3 = 1;
            PORTA_PA4 = 1;
            PORTA_PA5 = 0;
            break;
        default:
            counter_straight = counter_ss = counter_bs = counter_left = counter_right = counter_slope = 0;
            PORTA_PA0 = 0;
            PORTA_PA1 = 0;
            PORTA_PA2 = 0;
            PORTA_PA3 = 0;
            PORTA_PA4 = 0;
            PORTA_PA5 = 0;
            PORTA_PA6 = 0;
            PORTA_PA7 = 0;
            break;
    }
    
    //=============================================
    //                  strategy
    //=============================================
    
    direction_err = (Line_Center[39] - COLUMN_VALUE/2) + (campos - CAM_SERVO_ARR_MID);
    cam_err = abs(campos - CAM_SERVO_ARR_MID);
    valid_prediction_value = abs((ROW_VALUE - prediction) + (float)cam_err*0.5);
    if( dip1 )
    {
        scheme = 4;
        servo_factor = 0.7;
        servo_factor_d1 = 0.2;
        
        if( prediction > 30 )
        {
            LED_TURN_LEFT_ON();
            LED_TURN_RIGHT_ON();
            scheme = 8;
            servo_factor_d1 = 0;
        }
        else
        {
            LED_TURN_LEFT_OFF();
            LED_TURN_RIGHT_OFF();
            scheme = 4;
            servo_factor_d1 = 0.2;
        }
        
        if( valid_prediction_value < 5 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 5 && valid_prediction_value < 10 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 10 && valid_prediction_value < 15 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 15 && valid_prediction_value < 20 )
        {
            servo_factor = 0.5;
        }
        else if( valid_prediction_value >= 20 && valid_prediction_value < 30 )
        {
            servo_factor = 0.8;
        }
        else
        {
            servo_factor = 0.9;
        }
    }
    else if( dip2 )
    {
        scheme = 4;
        servo_factor = 0.7;
        servo_factor_d1 = 0.2;
        
        if( prediction > 30 )
        {
            LED_TURN_LEFT_ON();
            LED_TURN_RIGHT_ON();
            scheme = 9;
            servo_factor_d1 = 0;
        }
        else
        {
            LED_TURN_LEFT_OFF();
            LED_TURN_RIGHT_OFF();
            scheme = 4;
            servo_factor_d1 = 0.3;
        }
        
        if( valid_prediction_value < 5 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 5 && valid_prediction_value < 10 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 10 && valid_prediction_value < 15 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 15 && valid_prediction_value < 20 )
        {
            servo_factor = 0.5;
        }
        else if( valid_prediction_value >= 20 && valid_prediction_value < 30 )
        {
            servo_factor = 0.8;
        }
        else
        {
            servo_factor = 0.9;
        }
        
        if( routetype == 2 )
        {
            servo_factor_d1 = 0;
        }
    }
        
    else if( dip3 )
    {
        scheme = 4;
        servo_factor = 0.7;
        servo_factor_d1 = 0.3;
        
        if( prediction > 30 )
        {
            LED_TURN_LEFT_ON();
            LED_TURN_RIGHT_ON();
            scheme = 8;
            servo_factor_d1 = 0;
        }
        else
        {
            LED_TURN_LEFT_OFF();
            LED_TURN_RIGHT_OFF();
            scheme = 4;
        }
        
        if( valid_prediction_value < 5 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 5 && valid_prediction_value < 10 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 10 && valid_prediction_value < 15 )
        {
            servo_factor = 0.3;
        }
        else if( valid_prediction_value >= 15 && valid_prediction_value < 20 )
        {
            servo_factor = 0.5;
        }
        else if( valid_prediction_value >= 20 && valid_prediction_value < 30 )
        {
            servo_factor = 0.8;
        }
        else
        {
            servo_factor = 0.9;
        }
        
        if( routetype == 2 )
        {
            servo_factor_d1 = 0;
        }
        
        if( valid_prediction_value < 25 )
        {
            if( prediction <= 30 )
            {
                scheme = 3;
            }
        }
    }
    else if( dip4 )
    {
        scheme = 4;
        servo_factor = 0.7;
        servo_factor_d1 = 0.3;
        
        if( prediction > 30 )
        {
            LED_TURN_LEFT_ON();
            LED_TURN_RIGHT_ON();
            scheme = 8;
            servo_factor_d1 = 0;
        }
        else
        {
            LED_TURN_LEFT_OFF();
            LED_TURN_RIGHT_OFF();
            scheme = 4;
        }
        
        if( valid_prediction_value < 5 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 5 && valid_prediction_value < 10 )
        {
            servo_factor = 0.3;
            servo_factor_d1 = 0;
        }
        else if( valid_prediction_value >= 10 && valid_prediction_value < 15 )
        {
            servo_factor = 0.3;
        }
        else if( valid_prediction_value >= 15 && valid_prediction_value < 20 )
        {
            servo_factor = 0.5;
        }
        else if( valid_prediction_value >= 20 && valid_prediction_value < 30 )
        {
            servo_factor = 0.8;
        }
        else
        {
            servo_factor = 0.9;
        }
        
        if( routetype == 2 )
        {
            servo_factor_d1 = 0;
        }
        
        if( valid_prediction_value < 25 )
        {
            if( prediction <= 30 )
            {
                scheme = 3;
            }
        }
    }
    else
    {
        scheme = 4;
        servo_factor = 0.7;
        servo_factor_d1 = 0.3;
    }
            
    for( i = ROW_VALUE - 1; i >= 0; i-- )
    {
        weight = GetWeight(i, scheme);
        blackposavgtemp += Line_Center[i] * weight;
    }
    
    if( prediction < 20 )
    {
        blackposavg = Line_Center[ROW_VALUE - 1 - prediction];
    }
    else
    {
        blackposavg = (unsigned int)(blackposavgtemp + 0.5);
    }
}

float GetWeight(int i, int scheme)
{
    float weight;
    if( scheme == 1 )
    {
        if( i < 5 )
        {
            weight = 0;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.1;
        }
        else
        {
            weight = 0.1;
        }
    }
    else if( scheme == 2 )
    {
        if( i < 5 )
        {
            weight = 0;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.03;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.04;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.05;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.04;
        }
        else
        {
            weight = 0.04;
        }
        //=============================
        //for 90 degree "S", with servo_factor=0.6,speed=500  
        /*
        if( i < 5 )
        {
            weight = 0.01;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0.04;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0.04;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.04;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.03;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.02;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.01;
        }
        else
        {
            weight = 0.01;
        }
        */
        //=================================
    }
    else if( scheme == 3 )
    {
        if( i < 5 )
        {
            weight = 0;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0.05;
        }
        else if( i >= 15 && i < 20 )
        {
            //weight = 0.05;
            weight = 0.1;
        }
        else if( i >= 20 && i < 25 )
        {
            //weight = 0.1;
            weight = 0.05;
        }
        else if( i >= 25 && i < 30 )
        {
            //weight = 0.05;
            weight = 0;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0;
        }
        else
        {
            weight = 0;
        }
    }
    else if( scheme == 4 )
    {
        if( i < 5 )
        {
            weight = 0;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.05;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.05;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.05;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.05;
        }
        else
        {
            weight = 0;
        }
    }
    else if( scheme == 8 )  //test scheme
    {
        if( i < 5 )
        {
            weight = 0.01;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0.03;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0.03;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.01;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.01;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.01;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.05;
        }
        else
        {
            weight = 0.05;
        }
    }
    else if( scheme == 9 )  //test scheme
    {
        if( i < 5 )
        {
            weight = 0.01;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0.01;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0.02;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.05;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.01;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.02;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.02;
        }
        else
        {
            weight = 0.06;
        }
    }
    else
    {
        if( i < 5 )
        {
            weight = 0;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0.04;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0.05;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.06;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.05;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0;
        }
        else
        {
            weight = 0;
        }
    }
    return weight;
}