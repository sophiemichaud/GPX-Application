
// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {

    // On page-load for files on server
    jQuery.ajax({
        type: 'get', //Request type
        dataType: 'json', //Data type - we will use JSON for almost everything 
        url: '/filelog', //The server endpoint we are connecting to

        success: function(data) {

            if (data.length == 0) {
                $('#file-log-body').prepend('<p>No files on the server. Use the form at the bottom of this panel to upload an existing GPX file or the "<i>Create GPX Panel</i>" below to create a new GPX file from scratch.</p>');
                console.log("No files on server");
            } else {

                $('#file-log-body').prepend("<p>Below is a summary of all the valid GPX files on the server:</p>");

                for (let i = 0; i < data.length; i++) {

                    let json = JSON.parse(data[i]);
                    if (json == "Invalid") {
                        console.log("Cannot upload selected file, invalid GPX");
                        alert("Cannot upload selected file, invalid GPX");
                        continue;
                    }

                    if (i >= 1) {
                        let x = $('#file-log-table').height();
                        $('#file-log-table').css("height", x + 5);
                    }

                    $('#file-log-table').append(
                        '<tr>' +
                        '<td><a href="' + json["fileName"] + '" download>' +
                        json["fileName"] +
                        '</a></td><td>' +
                        json["version"] + "</td><td>" +
                        json["creator"] + "</td><td>" +
                        json["numWaypoints"] + "</td><td>" +
                        json["numRoutes"] + "</td><td>" +
                        json["numTracks"] + "</td></tr>"
                    );

                    $('#file-log-menu').append(
                        '<option value>' +
                        json["fileName"] +
                        '</option>'
                    );

                    $('#add-route-menu').append(
                        '<option value>' +
                        json["fileName"] +
                        '</option>'
                    );

                    $('#query2-menu').append(
                        '<option value>' +
                        json["fileName"] +
                        '</option>'
                    );

                    $('#query4-menu').append(
                        '<option value>' +
                        json["fileName"] +
                        '</option>'
                    );

                    $('#query5-menu').append(
                        '<option value>' +
                        json["fileName"] +
                        '</option>'
                    );
                }
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            alert("Could not upload" + json["fileName"] + "to the server");
            console.log(error);
        }
    });

    // Update the route and track tables on file select from dropdown
    $('#file-log-menu').change(function() {

        $('#route-message').empty();
        $('#track-message').empty();

        // Gets the filename from the selected option from the dropdown
        let fileName = $('#file-log-menu option:selected').text();

        //deletes the table contents before proceeding but keeps the header
        $('#route-table-body tr').empty();
        $('#track-table-body tr').empty();

        jQuery.ajax({
            type: 'get', //Request type
            dataType: 'json', //Data type - we will use JSON for almost everything 
            url: '/routeDisplay/' + fileName, //The server endpoint we are connecting to

            success: function(data) {

                if (data.length == 0) {
                    $('#route-message').prepend("<p>No routes to display for " + fileName + "</p>");
                }

                for (let i = 0; i < data.length; i++) {

                    if (i >= 1) {
                        let x = $('#route-table').height();
                        $('#route-table').css("height", x + 5);
                    }

                    let index = i;
                    let name = data[i]["name"];

                    $('#route-table').append(
                        '<tr><td>' + (i + 1) +
                        '</td><td>' +
                        data[i]["name"] +
                        '</td><td>' +
                        data[i]["numPoints"] +
                        '</td><td>' +
                        data[i]["len"] +
                        '</td><td>' +
                        data[i]["loop"] +
                        '</td><td><button class="btn" data-toggle="modal" data-target="#exampleModalCenterRoute" onclick="viewRouteData(this)" data-string="' + fileName + '" data-index="' + i + '">View Data</button>' +
                        '</td><td><button class="btn" data-toggle="modal" data-target="#exampleModalCenterRouteRename" onclick="renameRouteFunction(this)" data-string="' + fileName + '" data-index="' + i + '">Rename</button>' +
                        '</td></tr>'
                    );
                }

                alert("Successfully displayed route properties for " + fileName);
                console.log("Successfully displayed route properties for " + fileName);

            },
            fail: function(error) {
                // Non-200 return, do something with error
                alert("Could not display routes for " + fileName);
                console.log("Could not display routes for " + fileName);
                console.log(error);
            }
        });

        jQuery.ajax({
            type: 'get', //Request type
            dataType: 'json', //Data type - we will use JSON for almost everything 
            url: '/trackDisplay/' + fileName, //The server endpoint we are connecting to

            success: function(data) {

                if (data.length == 0) {
                    $('#track-message').prepend("<p>No tracks to display for " + fileName + "</p>");
                }

                for (let i = 0; i < data.length; i++) {

                    if (i >= 1) {
                        let x = $('#track-table').height();
                        $('#track-table').css("height", x + 5);
                    }

                    var info = JSON.stringify(data[i]);

                    $('#track-table').append(
                        '<tr><td>' + (i + 1) +
                        '</td><td>' +
                        data[i]["name"] +
                        '</td><td>' +
                        data[i]["numPoints"] +
                        '</td><td>' +
                        data[i]["len"] +
                        '</td><td>' +
                        data[i]["loop"] +
                        '</td><td><button class="btn" data-toggle="modal" data-target="#exampleModalCenterTrack" onclick="viewTrackData(this)" data-string="' + fileName + '" data-index="' + i + '">View Data</button>' +
                        '</td><td><button class="btn" data-toggle="modal" data-target="#exampleModalCenterTrackRename" onclick="renameTrackFunction(this)" data-string="' + fileName + '" data-index="' + i + '">Rename</button>' +
                        '</td></tr>'
                    );
                }

                alert("Successfully displayed track properties for " + fileName);
                console.log("Successfully displayed track properties for " + fileName);
            },
            fail: function(error) {
                // Non-200 return, do something with error
                alert("Could not display tracks for " + fileName);
                console.log("Could not display tracks for " + fileName);
                console.log(error);
            }
        });

    });

    $('#create-gpx').submit(function(event) {

        // Cancel submit action prior to actual submission
        event.preventDefault();

        // Gets variables from form
        let fileName = $('#file-name').val() + ".gpx";
        let version = $('#version').val();
        let creator = $('#creator').val();

        // Get float for version string
        version = parseFloat(version);

        // Displays routes between specified coordinates
        jQuery.ajax({

            type: 'get',
            dataType: 'json',
            url: '/createGPX/',
            data: {
                fileName: fileName,
                version: version,
                creator: creator,
            },

            success: function(data) {

                let json = data;

                // Appends the successfully created file to appropriate sections of the application
                $('#file-log-table').append(
                    '<tr>' +
                    '<td><a href="' + fileName + '" download>' +
                    fileName +
                    '</a></td><td>' +
                    json["version"] + "</td><td>" +
                    json["creator"] + "</td><td>" +
                    json["numWaypoints"] + "</td><td>" +
                    json["numRoutes"] + "</td><td>" +
                    json["numTracks"] + "</td></tr>"
                );

                $('#file-log-menu').append(
                    '<option value>' +
                    fileName +
                    '</option>'
                );

                $('#add-route-menu').append(
                    '<option value>' +
                    fileName +
                    '</option>'
                );

                $('#query2-menu').append(
                    '<option value>' +
                    json["fileName"] +
                    '</option>'
                );

                $('#query4-menu').append(
                    '<option value>' +
                    json["fileName"] +
                    '</option>'
                );

                $('#query5-menu').append(
                    '<option value>' +
                    json["fileName"] +
                    '</option>'
                );

                alert("Succesfully created new GPX file!");
                console.log("Succesfully created new GPX file!");
            },
            fail: function(error) {
                // Non-200 return, do something with error
                alert("Could not create new GPX file.");
                console.log("Could not create new GPX file.");
                console.log(error);
            }
        });
    });

    $('#find-path').submit(function(event) {

        // Cancel submit action prior to actual submission
        event.preventDefault();

        // Get variables from the form
        let latitudeStart = $('#starting-latitude').val();
        let longitudeStart = $('starting-longitude').val();
        let latitudeEnd = $('ending-latitude').val();
        let longitudeEnd = $('ending-longitude').val();
        let comp = $('point-comparison').val();

        // Gets floats from form input
        latitudeStart = parseFloat(latitudeStart);
        longitudeStart = parseFloat(longitudeStart);
        latitudeEnd = parseFloat(latitudeEnd);
        longitudeEnd = parseFloat(longitudeEnd);
        comp = parseFloat(comp);

        // Displays routes between specified coordinates
        jQuery.ajax({

            type: 'get',
            dataType: 'json',
            url: '/routesBetween/',
            data: {
                latitudeStart: latitudeStart,
                longitudeStart: longitudeStart,
                latitudeEnd: latitudeEnd,
                longitudeEnd: longitudeEnd,
                comp: comp,
            },

            success: function(data) {

                for (let i = 0; i < data.length; i++) {

                    let json = JSON.parse(data[i]);

                    if (json != null) {

                        for (let j = 0; j < json.length; j++) {

                            if (j >= 1) {
                                let x = $('#path-route-table').height();
                                $('#path-route-table').css("height", x + 5);
                            }

                            $('#path-route-table').append(
                                '<tr><td>' + (j + 1) +
                                '</td><td>' +
                                data[j]["name"] +
                                '</td><td>' +
                                data[j]["numPoints"] +
                                '</td><td>' +
                                data[j]["len"] +
                                '</td><td>' +
                                data[j]["loop"] +
                                '</td><td><button class="btn" id="data-button">View Data</button>' +
                                '</td><td><button class="btn" id="rename-button">Rename</button>' +
                                '</td></tr>'
                            );
                        }
                    }
                }

                alert("Successfully displayed routes between specified coordinates");
                console.log("Successfully displayed routes between specified coordinates");
            },
            fail: function(error) {
                // Non-200 return, do something with error
                alert("Could not display routes between specified coordinates");
                console.log("Could not display routes between specified coordinates");
                console.log(error);
            }
        });

        // Displays tracks between specified coordinates
        jQuery.ajax({

            type: 'get',
            dataType: 'json',
            url: '/tracksBetween/',
            data: {
                latitudeStart: latitudeStart,
                longitudeStart: longitudeStart,
                latitudeEnd: latitudeEnd,
                longitudeEnd: longitudeEnd,
                comp: comp,
            },

            success: function(data) {

                for (let i = 0; i < data.length; i++) {

                    let json = JSON.parse(data[i]);

                    if (json != null) {

                        for (let j = 0; j < json.length; j++) {

                            if (j >= 1) {
                                let x = $('#path-track-table').height();
                                $('#path-track-table').css("height", x + 5);
                            }

                            $('#path-track-table').append(
                                '<tr><td>' + (j + 1) +
                                '</td><td>' +
                                data[j]["name"] +
                                '</td><td>' +
                                data[j]["numPoints"] +
                                '</td><td>' +
                                data[j]["len"] +
                                '</td><td>' +
                                data[j]["loop"] +
                                '</td><td><button class="btn" id="data-button">View Data</button>' +
                                '</td><td><button class="btn" id="rename-button">Rename</button>' +
                                '</td></tr>'
                            );
                        }
                    }
                }

                alert("Successfully displayed tracks between specified coordinates");
                console.log("Successfully displayed tracks between specified coordinates");
            },
            fail: function(error) {
                // Non-200 return, do something with error
                alert("Could not display tracks between specified coordinates");
                console.log("Could not display tracks between specified coordinates");
                console.log(error);
            }
        });

    });


    /***************************************** A4 CODE *********************************************/

    /****************************** Login functionality ************************************/
    $('#login-form').submit(function(event) {

        // Cancel submit action prior to actual submission
        event.preventDefault();

        let username = $('#username').val();
        let password = $('#password').val();
        let database = $('#database').val();

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/login/',
            data: {
                username: username,
                password: password,
                database: database,
            },

            success: function(data) {

                if(data === "Connected"){

                    // Alerts user to successful login
                    alert("Successfully logged in!");
                    $("#clear-data-button").prop('disabled', false);
                    $("#update-button").prop('disabled', false);
                    $('#store-all-files-button').prop('disabled', false);
                    $('#query1-sort-name').prop('disabled', false);
                    $('#query1-sort-length').prop('disabled', false);
                    $('#query2-sort-name').prop('disabled', false);
                    $('#query2-sort-length').prop('disabled', false);
                }
                else{

                    // Alerts user to unsuccessful login
                    alert("Login failed, please try again!");
                }

            },
            fail: function(error) {

                // Non-200 return, do something with error
                alert("Login failed, please try again!");
                console.log(error);
            }
        });


        /****************************** Clear all data functionality ************************************/
        $('#clear-data-button').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
    
            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/clearData/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                },
    
                success: function(data) {

                    console.log("All data has been cleared from database. Click the refresh button in the database status pane.");
                    alert("All data has been cleared from the database. Click the refresh button in the database status pane.");
                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not clear database.");
                    console.log(error);
                }
            });
            
        });

        /****************************** Database status functionality ************************************/
        $('#update-button').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
    
            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/updateStatus/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                },
    
                success: function(data) {

                    // Sends message to console that database status pane has been refreshed.
                    console.log("Database status pane has been refreshed!");

                    // Updates the paragraph in the card body to display results.
                    $('#status').html("Database has " + data[0] + " files, " + data[1] + " routes, and " + data[2] + " points.");
                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not update database status.");
                    console.log(error);
                }
            });
            
        });


        /****************************** Store All Files Functionality ************************************/
        $('#store-all-files-button').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
    
            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/storeAllFiles/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                },
    
                success: function(data) {

                    // Sends message to console that database status pane has been refreshed.
                    alert("All files stored in the database. Update the database status pane.");
                    console.log("All files stored in the database. Update the database status pane.");

                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not store all files. Try again.");
                    console.log(error);
                }
            });
            
        });

        /****************************** Store All Files Functionality ************************************/
        $('#query1-sort-name').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
            let sort = "route_name";
            $('#query1-table-body tr').empty();

            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/query1/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                    sort: sort,
                },
    
                success: function(data) {

                    for(var i = 0; i < data.length; i++){

						$('#query1-table-body').append(
                            "<tr><td>" 
                            + data[i]["name"] 
                            + "</td><td>" 
                            + data[i]["len"]
                            + "</td><td>"
                            + data[i]["id"]
                            + "</td>"
                        );
					}

                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not store all files. Try again.");
                    console.log(error);
                }
            });

        });

        $('#query1-sort-length').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
            $('#query1-table-body tr').empty();
            let sort = "route_len";

            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/query1/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                    sort: sort,
                },
    
                success: function(data) {

                    for(var i = 0; i < data.length; i++){

						$('#query1-table-body').append(
                            "<tr><td>" 
                            + data[i]["name"] 
                            + "</td><td>" 
                            + data[i]["len"]
                            + "</td><td>"
                            + data[i]["id"]
                            + "</td>"
                        );
					}

                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not store all files. Try again.");
                    console.log(error);
                }
            });

        });

        $('#query2-sort-length').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
            $('#query2-table-body tr').empty();
            let sort = "route_len";
            let fileName = $('#query2-menu option:selected').text();

            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/query2/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                    sort: sort,
                    fileName: fileName,
                },
    
                success: function(data) {

                    for(var i = 0; i < data.length; i++){

						$('#query2-table-body').append(
                            "<tr><td>" 
                            + data[i]["name"] 
                            + "</td><td>" 
                            + data[i]["len"]
                            + "</td><td>"
                            + data[i]["id"]
                            + "</td><td>"
                            + fileName
                            + "</td>"
                        );
					}

                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not store all files. Try again.");
                    console.log(error);
                }
            });

        });

        $('#query2-sort-name').click(function(event) {

            // Cancel submit action prior to actual submission
            event.preventDefault();
            $('#query2-table-body tr').empty();
            let sort = "route_name";
            let fileName = $('#query2-menu option:selected').text();

            $.ajax({
    
                type: 'get',
                dataType: 'json',
                url: '/query2/',
                data: {
                    username: username,
                    password: password,
                    database: database,
                    sort: sort,
                    fileName: fileName,
                },
    
                success: function(data) {

                    for(var i = 0; i < data.length; i++){

						$('#query2-table-body').append(
                            "<tr><td>" 
                            + data[i]["name"] 
                            + "</td><td>" 
                            + data[i]["len"]
                            + "</td><td>"
                            + data[i]["id"]
                            + "</td><td>"
                            + fileName
                            + "</td>"
                        );
					}

                },
                fail: function(error) {
    
                    // Non-200 return, do something with error
                    alert("Could not store all files. Try again.");
                    console.log(error);
                }
            });

        });
    });

});





// A3 Functions

function viewRouteData(element) {
    var fileName = $(element).data("string");
    var index = $(element).data("index");

    jQuery.ajax({
        type: 'get', //Request type
        dataType: 'json', //Data type - we will use JSON for almost everything 
        url: '/routeDisplayData',
        data: {
            fileName: fileName,
            index: index,
        },

        success: function(data) {

            let json = JSON.parse(data);

            if (json.length == 0) {
                $('#modal-body-route').prepend(
                    'No additional data belongs to this route'
                );
            }

            $('#route-data-table-body tr').empty();
            for (let i = 0; i < json.length; i++) {

                $('#route-data-table-body').append(
                    '<tr>' +
                    '<td>' +
                    json[i].name +
                    '</td><td>' +
                    json[i].value + "</td>" +
                    '</tr>'
                );
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            alert("Could not display data.");
            console.log(error);
        }

    });
}

function viewTrackData(element) {
    var fileName = $(element).data("string");
    var index = $(element).data("index");

    jQuery.ajax({
        type: 'get', //Request type
        dataType: 'json', //Data type - we will use JSON for almost everything 
        url: '/trackDisplayData',
        data: {
            fileName: fileName,
            index: index,
        },

        success: function(data) {

            let json = JSON.parse(data);

            if (json.length == 0) {
                $('#modal-body-track').prepend(
                    'No additional data belongs to this route'
                );
            }

            $('#track-data-table-body tr').empty();
            for (let i = 0; i < json.length; i++) {

                $('#track-data-table-body').append(
                    '<tr>' +
                    '<td>' +
                    json[i].name +
                    '</td><td>' +
                    json[i].value + "</td>" +
                    '</tr>'
                );
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            alert("Could not display data.");
            console.log(error);
        }

    });
}

function renameRouteFunction(element) {
}

function renameTrackFunction(data) {
    $('#rename-track-form').show();
}

