let map, infoWindow;
var savedPlaces = new Array();
let markers = [];


function initMap () {
    infoWindow = new google.maps.InfoWindow();
    map = new google.maps.Map(document.getElementById("map"), {
        center: { lat: 0, lng: 0},
        zoom: 3,
    });
    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(
            (position) => {
                const pos = {
                    lat: position.coords.latitude,
                    lng: position.coords.longitude,
                };
                infoWindow.setPosition(pos);

                map.setCenter(pos);
                map.setZoom(10);
            },
            () => {
                handleErrLoc(true, infoWindow, map.getCenter());
            }
        );
    } else {
        handleErrLoc(false, infoWindow, map.getCenter());
    }

    const Loc = document.getElementById("findInput");
    const search = new google.maps.places.SearchBox(Loc);

    map.addListener("bounds_changed", () => {
        search.setBounds(map.getBounds());
    });


    search.addListener("places_changed", () => {
        const places = search.getPlaces();
        centerHandler(places, map)
    });

    document.getElementById("addPos").addEventListener("click", () => {
        const places = search.getPlaces();
        addLocation(places);
    });

    const geocoder = new google.maps.Geocoder();

    google.maps.event.addListener(map, 'click', function (event) {
        getPlaceFromGeocode(event, geocoder, map, infoWindow);
    });

    google.maps.event.addListener(map, 'rightclick', () => {
        savedPlaces = [];
        refreshPlaces();
    });

}

function findSignificant(map, place){
    const search = new google.maps.places.PlacesService(map);
    var request = {
        location: place.geometry.location,
        radius: 10000,
        query: "point of interest"
    };
    search.textSearch(request, significantCallback);
}

function significantCallback(results, status){
    if (status == google.maps.places.PlacesServiceStatus.OK) {
        if (results){
            openGallery(results);
        }
    }
}

function openGallery(results){
    photoIndex = 1;
    document.getElementById("Gallery").style.display = "block";

    var newgallery = document.createElement("div");
    newgallery.setAttribute("id", "gallery");
    newgallery.setAttribute("class", "gallery");
    document.getElementById("Gallery").appendChild(newgallery);

    var closeBut = document.createElement("span");
    closeBut.setAttribute("class", "close");
    closeBut.innerHTML = "&times;";
    closeBut.setAttribute("onclick", "closeGallery()");
    newgallery.appendChild(closeBut);

    var galContent = document.createElement("div");
    galContent.setAttribute("class", "photoElement");
    newgallery.appendChild(galContent);

    var next = document.createElement("a");
    next.setAttribute("class", "next");
    next.innerHTML = "&#10095;";
    next.setAttribute("onclick", "rotate(1)");
    newgallery.appendChild(next);

    var prev = document.createElement("a");
    prev.setAttribute("class", "prev");
    prev.innerHTML = "&#10094;";
    prev.setAttribute("onclick", "rotate(-1)");
    newgallery.appendChild(prev);

    results.forEach((result) => {
        if (result.photos){
            savedInterest.push(result);
            var newPhoto = document.createElement("div");
            newPhoto.setAttribute("class", "photo");
            galContent.appendChild(newPhoto);

            var source = result.photos[0].getUrl({maxHeight: result.photos[0].height});
            var image = document.createElement("img");
            image.setAttribute("src", source);
            newPhoto.appendChild(image);
        }
    });

    var caption = document.createElement("div");
    caption.setAttribute("class", "caption");
    newgallery.appendChild(caption);

    var caption_text = document.createElement("p");
    caption_text.setAttribute("id", "caption");
    caption.appendChild(caption_text);

    showPhoto(photoIndex);
}

function closeGallery(){
    savedInterest = [];
    var oldSaved = document.getElementById("gallery");
    if(oldSaved){
        oldSaved.parentNode.removeChild(oldSaved);
    }
}

function rotate(num){
    showPhoto(photoIndex += num);
}

function showPhoto(index){
    var photos = document.getElementsByClassName("photo");
    if (index < 1){
        photoIndex = photos.length;
    }else if (photoIndex > photos.length){
        photoIndex = 1;
    }else{}

    var counter = 1;
    Array.prototype.forEach.call(photos, (photo) => {
        if (counter == photoIndex){
            photo.style.display = 'block';
            var caption_text = document.getElementById("caption");
            caption_text.innerHTML = savedInterest[photoIndex-1].name;
        }else{
            photo.style.display = 'none';
        }
        counter += 1;
    });

    console.log(photoIndex);
}

function getPlaceFromGeocode(event, geocoder, map, infowindow){
    geocoder.geocode({'latLng': event.latLng}, (result, status) => {
        if(status == google.maps.GeocoderStatus.OK && result[0]){
            const search = new google.maps.places.PlacesService(map);
            var request = {query: result[0].formatted_address};
            search.textSearch(request, callbackGeocode);
        }else{
            handleErrLoc(true, infoWindow, map.getCenter());
        }
    })
}

function callbackGeocode(results, status){
    if (status == google.maps.places.PlacesServiceStatus.OK) {
        if (results){
            addLocation(results);
        }
    }
}

function handleErrLoc (geoLoc, infoWindow, pos){
    infoWindow.setPosition(pos);
    infoWindow.setContent(
        geoLoc ? "Geolocation not found!" : "Browser does not support geolocation!");
    infoWindow.open(map);

}

function addLocation(places){
    console.log(savedPlaces);
    if(places){
        savedPlaces.push(places[0]);
        refreshPlaces();
    }
}

function delLocation(item){
    savedPlaces.splice(item.substring(3), 1)
    refreshPlaces();
}

var $ = function (selector) {
    return document.querySelector(selector);
};

function refreshPlaces(){
    var oldSaved = document.getElementById("SavedPlaces");
    if(oldSaved){
        oldSaved.parentNode.removeChild(oldSaved);
    }
    var newSaved = document.createElement("div");
    newSaved.setAttribute("id", "SavedPlaces");
    document.getElementById("bottomnav").appendChild(newSaved);

    let index = 0;
    markers.forEach((marker) => {
        marker.setMap(null);
    });
    markers = [];
    savedPlaces.forEach((place) => {
        let newPar = document.createElement("div");
        newPar.setAttribute("id", "place"+index);
        newPar.setAttribute("class", "place");

        let newPlace = document.createElement("div");
        newPlace.innerHTML = place.formatted_address;

        let newPlaceDel = document.createElement("button");

        newPlace.addEventListener("click", () => {
            findSignificant(map, place);
        });

        newPlaceDel.textContent = "Delete";
        newPlaceDel.setAttribute("id", "del" + index);
        newPlaceDel.setAttribute("class", "delete");

        newPar.appendChild(newPlace);
        newPar.appendChild(newPlaceDel);

        newSaved.appendChild(newPar);

        var marker =  new google.maps.Marker({
            map,
            title: place.name,
            position: place.geometry.location,
        });

        google.maps.event.addListener(marker, 'click', () => {
            delLocation(newPlaceDel.id);
        });
        
        markers.push(marker);


        index += 1;
    });

    document.querySelectorAll('.delete').forEach(item =>{
        item.addEventListener('click', event =>{
            delLocation(item.id);
        });
    });

}

function centerHandler(places,map){
    if (places){
        if(places.length == 0) return;
        const newBounds = new google.maps.LatLngBounds();

        places.forEach((place) => {
            if (place.geometry && place.geometry.location){
                place.geometry.viewport ? newBounds.union(place.geometry.viewport) : newBounds.extend(place.geometry.location);
            }else{
                return;
            }
        });
        map.fitBounds(newBounds);
    }else{
        return;
    }
}

