// Put all onload AJAX calls here, and event listeners

jQuery(document).ready(function() {
    // On page-load AJAX Example


    /**call this after any modification to the list of files is done -> will weed out duplicates and broken files */
    /**decided not to do most of this in the upload function itself because I didn't realize directory always started empty */
    /** used to aqcuire preexisting files in directory */ 
    jQuery.ajax ({
        type: 'get',
        dataType: 'text',//receiving number of files in text format (convert back with Number()) 
        url: '/endpoint3',
        data: {
            data1: "Value",
        },
        success: function (data){
            var max = Number (data) 
            console.log(max); 
            var i = 0; 
            for (i; i < max; i++){

                jQuery.ajax ({
                    type: 'get', 
                    dataType: 'json',
                    url: '/endpoint2', 
                    data: {
                        dataIdx: i,
                    },
                    success: function (data){
                        if (data == null){//this data is JSON strings for the table 
                            //deleted file
                        }
                        else {
                            //document.getElementById('addRow').onclick = function(){
                            //let row = table.insertRow(-1) ; //create teh entire row for a file 

                            var table = document.getElementById ('table'); 

                            var row = table.insertRow(-1);

                            let cell1 = row.insertCell(0);

                            var a = document.createElement('a'); 
                            a.href = '/uploads/' + data.name; 
                            a.download = data.name; 
                            a.innerHTML = data.name; 
                            cell1.appendChild(a); 

                            let cell2 = row.insertCell(1);
                            cell2.innerHTML = data.version; 

                            let cell3 = row.insertCell(2); 
                            cell3.innerHTML = data.creator;

                            let cell4 = row.insertCell(3); 
                            if (data.wptNum == null){
                                cells4.innerHTML = "0"; 
                            }
                            else {
                                cell4.innerHTML = data.wptNum;
                            }
                             

                            let cell5 = row.insertCell(4); 
                            if (data.rteNum == null){
                                cell5.innerHTML = "0"; 
                            }
                            else {
                                cell5.innerHTML = data.rteNum; 
                            }
                            
                            let cell6 = row.insertCell(5);
                            if (data.trackNum == null){
                                cell6.innerHTML = "0"; 
                            }
                            else {
                                cell6.innerHTML = data.trackNum; 
                            }

                            var opt = document.createElement ("option");
                            opt.innerHTML = data.name; 
                            opt.value = data.name; 
                            var selection = document.getElementById ('files');
                            selection.appendChild (opt); 


                            if (i >= 3){
                                document.getElementById('divTable').style.overflowY = "scroll";
                            }

                        }
                        
                    },
                    fail: function (error){
            
                    }
                });
            }
        },
        fail: function (error){

        }
    });

})

/*

    document.getElementById('files').onchange = function(){
        if (document.getElementById('files').selectedIndex != 0){
            //console.log (document.getElementById('files').value); 
            //console.log (document.getElementById('files').selectedIndex);e

    
            var index  = document.getElementById('files').selectedIndex; 
            var value = document.getElementById('files').selectedOptions[index].value; 

            jQuery.ajax ({
                type: 'get',
                dataType: 'json',//receiving number of files in text format (convert back with Number()) 
                url: '/endpoint3',
                data: {
                    dataX = value,
                },
                success: function(data){
                    console.log (data); 
                },
                fail: function (Error){

                }
            });
        } else {
            //clrear entries 
        }
    }
   
});*/


            /*if (document.getElementById.('files').length < 10){
                document.getElementById('divtable2').style.overFlowY = "scroll"; 
            } -> idk if this is for total drop down or table */
            /*jQuery.ajax ({
                type: 'get',
                dataType: 'text',//receiving number of files in text format (convert back with Number()) 
                url: '/endpoint5',
                data: {
                    data1: "Value",
                },
                success: function (data){*/

   /* jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            data1: "Value 1",
            data2:1234.56
        },
        success: function (data) {
            
                jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
                //We write the object to the console to show that the request was successful
                console.log(data); 
    
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#blah').html("On page load, received error from server");
                console.log(error); 
            }
    });*/

    /*
    document.getElementById('files').onchange = function(){
        if (document.getElementById('files').selectedIndex != 0){
            console.log (document.getElementById('files').value); 
            console.log (document.getElementById('files').selectedIndex);ee //returns to ee 
        }
    }*/


    /*
    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){ //don't have to use this, can just use the event listeners with the ids like you've already looked at 
        $('#blah').html("Form has data: "+$('#entryBox').val()); 
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
});*/