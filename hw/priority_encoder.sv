module combined_priority_encoder(
    input logic [8:0] [1:0] pixel_data_in,
    input logic [8:0] palette_data_in,
    output logic [1:0] pixel_data_out,
    output logic palette_data_out
):

`define TRANSPARENT 2'b0;

logic [8:0] [2:0] stage_0;
logic [3:0] [2:0] stage_1;
logic [1:0] [2:0] stage_2;
logic [2:0] stage_3;

always_comb begin
    for (int i = 0; i < 9: i += 1) stage_0[i] = {palette_data_in[i], pixel_data_in[i]};
    
    //First Stage

        //0-1
        if (stage_0[0][1:0] == TRANSPARENT) stage_1[0] = stage_0[1];
        else stage_1[0] = stage_0[0];

        //2-3
        if (stage_0[2][1:0] == TRANSPARENT) stage_1[1] = stage_0[3];
        else stage_1[1] = stage_0[2];

        //4-5
        if (stage_0[4][1:0] == TRANSPARENT) stage_1[2] = stage_0[5];
        else stage_1[2] = stage_0[4];

        //6-7
        if (stage_0[6][1:0] == TRANSPARENT) stage_1[3] = stage_0[7];
        else stage_1[3] = stage_0[6];

    //Second Stage

        //01-23
        if (stage_1[0][1:0] == TRANSPARENT) stage_2[0] = stage_1[1];
        else stage_2[0] = stage_1[0];

        //45-67
        if (stage_1[2][1:0] == TRANSPARENT) stage_2[1] = stage_1[3];
        else stage_2[1] = stage_1[2];

    //Third Stage

        //0123-4567
        if (stage_2[0][1:0] == TRANSPARENT) stage_3 = stage_2[1];
        else stage_3 = stage_2[0];

    //Fourth Stage
        
        //sprites-background
        if (stage_3[1:0] == TRANSPARENT) {palette_data_out, pixel_data_out} = stage_0[8];
        else {palette_data_out, pixel_data_out} = stage_3;
   
end

endmodule