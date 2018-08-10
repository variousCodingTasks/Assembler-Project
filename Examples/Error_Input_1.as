;demonstrates different potential errors
;with directrives

.struct 8 , "testing..."
.struct 8 , "testing..." ;
.struct "testing...
.struct 8  "testing..."
.struct 8 
.struct 8 , 
.struct 8 , "testing...
.struct 8 , testing..."
.struct 			
.struct  , "testing..."

.string "abcd"
.string "abcd
.string abcd"
.string "abcd" ;
.string

.data 1 , 2, -5 ,  		-61
.data
.data 1 , 2 -5 ,  		-61
.data 1 , 2, - ,  		-61
.data 1 , 2 - ,  		-61
.data 1 , 2, -5 ,  		-61 +
.data 1 , 2, -5 ,  		-61 -
.data 1 , 2, -5 ,  		-61 ,
.data 1 , 2, -5 ,  		-61 '
.data ,
.data +
.data '
.data 1 , 2,  ,  		-61