# ESP8266 webui

built like an Arduino library provides a web interface for [big_seven](https://github.com/putyn/big_seven) and [naked nixie](https://github.com/putyn/nixie-clock) clocks

### Dependencies

```
nodejs and glup for web development
Arduino IDE with ESP8266 core for library development
free time
```

### Library: install/ usage
Clone/download library and move in Arduino library directories, install web development tools and generate necessary files, see example in:
```
Examples\webui
```

### Web development: install/ usage
Folder structure:
``` 
|- src/
      |- css/
      |- fonts/
      |- img/ 
      |- index.html
      |- js/ 
|- dist/
      | - build/ 
|- gulpfile.js
|- server.js
|- node_modules/
|- package.json
```

Install web server and gulp dependencies:
```
cd html
npm install
node server
```
build **seven** or **nixie** webui (see gulp tasks) and navigate to:
```
http://localhost:8080
```

gulp tasks:
```
seven - build webui, for big_seven
seven_watch - same as seven but watches the index file
nixie - build webui, for naked nixie
nixie_watch - same as nixie, watches files and rebuilds
build_fs - build for ESP8266
clean:[all|build] - cleans dist folder/ build folder
```

## Built With

* [bootstrap](http://getbootstrap.com/) - The most popular HTML, CSS, and JS library in the world.
* [bootswatch](https://bootswatch.com/) - Free themes for Bootstrap
* [jquery](https://jquery.com/) - jQuery is a fast, small, and feature-rich JavaScript library. 
* [nprogress](http://ricostacruz.com/nprogress/) - A nanoscopic progress bar. Featuring realistic trickle animations to convince your users that something is happening!
* [noUiSlider](https://refreshless.com/nouislider/) - noUiSlider: lightweight JavaScript range slider

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* **SO** for being supportive 
* [Xose Pérez](http://tinkerman.cat/optimizing-files-for-spiffs-with-gulp/) - Optimizing files for SPIFFS with Gulp
