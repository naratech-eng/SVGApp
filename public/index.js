var currentFile;

/* first define some functions */
function updateViewPanel() {
	if ($('#viewSelect option:selected').text() !== undefined) {
		var parameters = { file: $('#viewSelect option:selected').text() };

		console.log("updating view for " + $('#viewSelect option:selected').text());

		$.ajax({
			type: 'get',
			dataType: 'json',
			url: "/showsvgfile",
			data: parameters,

			success: function (info) {
				$("#viewImg").attr("src", $('#viewSelect option:selected').text());
				// var editTitleButton = $('<button id="editTitle" value="edit" class="btn btn-primary">edit</button>');
				if (info.ret) {
					currentFile = info.ret;

					if (info.ret.title) {
						$("#viewTitle").text(info.ret.title);
					}
					// $("#viewTitle").append(editTitleButton);
					//
					if (info.ret.description) {
						$("#viewDesc").text(info.ret.description);
					}
					/* clear all information regarding rects, circles, paths, gorups */
					$("#viewTable tbody").empty();

					/* clear information regarding other attributes */
					$("#viewSelectShapeForAttr").empty();

					var selected = " selected";

					/* Adding descriptions of rectangles */
					var i = 0;
					if (info.ret.rects) {
						info.ret.rects.forEach(rect => {
							var units = "";

							if (rect.units != "") {
								/* combining units + space for separation here */
								units = " " + rect.units;
							}

							var rectDesc = "x: " + rect.x + units + " y: " + rect.y + units + " width: " + rect.w + units + " height: " + rect.h + units;

							console.log("adding details for rect: " + rectDesc);

							$('#viewTable tbody').append(
								"<tr><td>" + i + "</td><td>Rectangle</td><td>" + rectDesc + "</td><td>" + rect.numAttr + "</td></tr>"
							);
							$('#viewSelectShapeForAttr').append($("<option" + selected + "></option>").text("Rect " + i));
							i++;

							selected = "";
						});
					}

					/* Adding descriptions of circles */
					i = 0;

					if (info.ret.circles) {
						info.ret.circles.forEach(circ => {
							var units = "";

							if (circ.units != "") {
								/* combining units + space for separation here */
								units = " " + circ.units;
							}

							var circDesc = "cx: " + circ.cx + units + " y: " + circ.cy + units + " radius: " + circ.r + units;

							console.log("adding details for circle: " + circDesc);

							$('#viewTable tbody').append(
								"<tr><td>" + i + "</td><td>Circle</td><td>" + circDesc + "</td><td>" + circ.numAttr + "</td></tr>"
							);
							$('#viewSelectShapeForAttr').append($("<option" + selected + "></option>").text("Circle " + i));
							i++;
						});
					}

					/* Adding descriptions of paths */
					i = 0;
					if (info.ret.paths) {
						info.ret.paths.forEach(path => {
							var pathDesc = "path data = " + path.d;

							console.log("adding details for path: " + pathDesc);

							$('#viewTable tbody').append(
								"<tr><td>" + i + "</td><td>Path</td><td>" + pathDesc + "</td><td>" + path.numAttr + "</td></tr>"
							);
							$('#viewSelectShapeForAttr').append($("<option" + selected + "></option>").text("Path " + i));
							i++;
						});
					}

					/* Adding descriptions of groups */
					i = 0;
					if (info.ret.groups) {
						info.ret.groups.forEach(group => {
							var groupDesc = group.children + " child elements";

							console.log("adding details for group: " + groupDesc);

							$('#viewTable tbody').append(
								"<tr><td>" + i + "</td><td>Group</td><td>" + groupDesc + "</td><td>" + group.numAttr + "</td></tr>"
							);
							$('#viewSelectShapeForAttr').append($("<option" + selected + "></option>").text("Group " + i));
							i++;
						});
					}


				}
			},
			fail: function (error) {
				console.log("error: " + error);
			}
		});
	}

	return false;
}

function updateAttrList(shape) {
	/* clear all information regarding rects, circles, paths, gorups */
	$("#showAttr tbody").empty();

	if (currentFile !== undefined) {
		/* we have data for the current svg file,
		 * -> find the right shape */

		var index = parseInt(shape.substr(shape.indexOf(" "), shape.length));
		var shapeObject;

		if (~shape.toLowerCase().indexOf("rect")) {
			shapeObject = currentFile.rects[index];
		}
		if (~shape.toLowerCase().indexOf("circ")) {
			shapeObject = currentFile.circles[index];
		}
		if (~shape.toLowerCase().indexOf("path")) {
			shapeObject = currentFile.paths[index];
		}
		if (~shape.toLowerCase().indexOf("group")) {
			shapeObject = currentFile.groups[index];
		}

		if (shapeObject) {
			var i = 0;
			Object.keys(shapeObject.other).forEach(attr => {
				$("#showAttr tbody").append(
					"<tr><td>Attribute " + i + "</td><td>" + shapeObject.other[i].name + "</td><td>" + shapeObject.other[i].value + "</td></tr>"
				);
				i++;
			});
		}
	}

}

$('.editable').click(function (e) {
	e.stopPropagation();
	var value = $(this).html();
	var id = $(this).attr('id');

	console.log("id: " + id);

	if (id === 'viewTitle') {
		field = 'title';
	}

	if (id === 'viewDesc') {
		field = 'desc';
	}

	updateVal(this, value, field);
});

function updateVal(currentEle, value, field) {
	var old = value;
	$(currentEle).html('<input class="thVal" maxlength="255" type="text" width="2" value="' + value + '" />');
	$(".thVal", currentEle).focus().keyup(function (event) {
		if (event.keyCode == 13) {
			$(currentEle).html($(".thVal").val().trim());
			console.log("updating " + field + " to " + this.value);

			var parameters = {};
			var endpoint = "";

			if (field === 'title') {
				endpoint = "/updatetitle";
				var parameters = {
					file: $('#viewSelect option:selected').text(),
					title: this.value
				};
			}

			if (field === 'desc') {
				endpoint = "/updatedescription";
				var parameters = {
					file: $('#viewSelect option:selected').text(),
					desc: this.value
				};
			}

			$.ajax({
				type: 'put',
				dataType: 'json',
				url: endpoint, //"/updatetitle",
				data: parameters,

				success: function (info) {

				},
				fail: function (err) {
					console.log("error: " + err);
				}
			});

		}
	}).click(function (e) {
		e.stopPropagation();
	});

	$(document).click(function () {
		$(".thVal").replaceWith(function () {
			console.log("discarding changes");
			return old;
		});
	});
}

/* event handlers before document ready, so they are executed
 * when the document loads */
$("#viewSelect").on('change', function () {
	updateViewPanel();
});

$("#viewSelectShapeForAttr").on('change', function () {
	console.log("loading attributes for shape: " + $("#viewSelectShapeForAttr option:selected").text());
	updateAttrList($("#viewSelectShapeForAttr option:selected").text());
});

// Put all onload AJAX calls here, and event listeners
$(document).ready(function () {
	// On page-load AJAX Example

	$.ajax({
		type: 'get',
		dataType: 'json',
		url: "/listsvgfiles",
		success: function (data) {
			/* doing few things at once here:
			 * filling table with file details
			 * adding options to the select box for the view panel
			 * adding options to the select box for the add shape panel 
			 * adding options to the select box for the edit shape panel 
			 */
			var table = $("#filelist");
			var rectCount = 0;

			$('#viewSelect').empty();

			/* set to selected for the first element */
			var selected = " selected";
			data.filelist.forEach(file => {

				console.log("received file: " + file);
				table.append('<tr><td><a href="' + file.file + '" download><img src="' + file.file + '" alt="..." class="img-thumbnail"></a></td><td><a href="' + file.file + '" download>' + file.file + '</a></td><td>' + file.size + '</td><td>' + file.rects + '</td><td>' + file.circles + '</td><td>' + file.paths + '</td><td>' + file.groups + '</td></tr>');
				$('#viewSelect').append($("<option" + selected + "></option>").text(file.file));
				$('#viewSelectAddRect').append($("<option" + selected + "></option>").text(file.file));
				$('#viewSelectAddCircle').append($("<option" + selected + "></option>").text(file.file));
				$('#viewSelectScale').append($("<option" + selected + "></option>").text(file.file));
				/* set to empty string, so only first element is selected */
				selected = "";
			});
		},
		fail: function (error) {
			console.log("error: " + error);
		},
		complete: function () {
			$('#viewSelect').change();
		}
	});

	// Event listener form example , we can use this instead explicitly listening for events
	// No redirects if possible
	$('#someform').submit(function (e) {
		$('#blah').html("Form has data: " + $('#entryBox').val());
		e.preventDefault();
		//Pass data to the Ajax call, so it gets passed to the server
		$.ajax({
			//Create an object for connecting to another waypoint
		});
	});

	$('#inputGroupFile02').on('change', function () {
		//get the file name
		var fileName = $(this).val();
		//replace the "Choose a file" label
		$(this).next('.custom-file-label').html(fileName);
	});

	$('#countRectangles').on('load', function () {
		/* empty parameters for now */
		var parameters = {};
		$.get('/countRectangles', parameters, function (count) {
			alert("read count: " + count);
			$(this).html(count);
		})
	});
});

$("#fileView").on('submit', function () {
	var parameters = { file: $('#viewSelect option:selected').text() };
	console.log("select changed value to: " + $('#viewSelect option:selected').text());

	$.ajax({
		type: 'get',
		dataType: 'json',
		url: "/showsvgfile",
		data: parameters,

		success: function (info) {
			$("#viewImg").attr("src", $('#viewSelect option:selected').text());
			$("#viewTitle").text(info.ret.title);
			$("#viewDesc").text(info.ret.description);
			/* clear all information regarding rects, circles, paths, gorups */
			$("#viewTable tbody").empty();

			/* Adding descriptions of rectangles */
			var i = 0;
			if (info.ret.rects) {
				info.ret.rects.forEach(rect => {
					var units = "";

					if (rect.units != "") {
						/* combining units + space for separation here */
						units = " " + rect.units;
					}

					var rectDesc = "x: " + rect.x + units + " y: " + rect.y + units + " width: " + rect.w + units + " height: " + rect.h + units;

					console.log("adding details for rect: " + rectDesc);

					$('#viewTable tbody').append(
						"<tr><td>" + i + "</td><td>Rectangle</td><td>" + rectDesc + "</td><td>" + rect.numAttr + "</td></tr>"
					);
					i++;
				});
			}

			/* Adding descriptions of circles */
			i = 0;

			if (info.ret.circles) {
				info.ret.circles.forEach(circ => {
					var units = "";

					if (circ.units != "") {
						/* combining units + space for separation here */
						units = " " + circ.units;
					}

					var circDesc = "cx: " + circ.cx + units + " y: " + circ.cy + units + " radius: " + circ.r + units;

					console.log("adding details for circle: " + circDesc);

					$('#viewTable tbody').append(
						"<tr><td>" + i + "</td><td>Circle</td><td>" + circDesc + "</td><td>" + circ.numAttr + "</td></tr>"
					);
					i++;
				});
			}

			/* Adding descriptions of paths */
			i = 0;
			if (info.ret.paths) {
				info.ret.paths.forEach(path => {

					/* the path data are truncated to 64 characters here, 
					 * as requested in the assignment */
					var pathDesc = "path data = " + path.d.substring(0, 64);

					console.log("adding details for path: " + pathDesc);

					$('#viewTable tbody').append(
						"<tr><td>" + i + "</td><td>Path</td><td>" + pathDesc + "</td><td>" + path.numAttr + "</td></tr>"
					);
					i++;
				});
			}

			/* Adding descriptions of groups */
			i = 0;

			if (info.ret.groups) {
				info.ret.groups.forEach(group => {
					var groupDesc = group.children + " child elements";

					console.log("adding details for group: " + groupDesc);

					$('#viewTable tbody').append(
						"<tr><td>" + i + "</td><td>Group</td><td>" + groupDesc + "</td><td>" + group.numAttr + "</td></tr>"
					);
					i++;
				});
			}

		},
		fail: function (error) {
			console.log("error: " + error);
		}
	});

	return false;
});

