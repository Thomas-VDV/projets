from pytube import YouTube
from pytube  import Playlist
import requests
from moviepy.editor import VideoFileClip
import os
import eyed3 
from eyed3.id3.frames import ImageFrame
import subprocess
import json
import yt_dlp
import os
import argparse
from pytube import Channel
import scrapetube
import re

def delete_file(file):
    if os.path.exists(file):
        os.remove(file)

def replace_slash(string):
    new_string = ""
    for caractere in string:
        if ord(caractere) == 47:
            new_string += "-"
        if ord(caractere) == ':':
            new_string += "-"
        else:
            new_string += caractere
    return new_string

def recover_metadata(url):
    metadata = {}
    video = YouTube(url)
    metadata["title"] = video.title
    metadata["title"] = replace_slash(metadata["title"])
    metadata["artist"] = video.author
    metadata["album"] = "Single"
    metadata["miniature_url"] = video.thumbnail_url

    commande = "youtube-dl --dump-json "
    process = subprocess.Popen(commande+url, shell=True,stdout=subprocess.PIPE, stderr=subprocess.PIPE) 
    output, error = process.communicate()
    if(error == b''):
        video_metadata = json.loads(output.decode("utf-8"))
        metadata["album"] = video_metadata.get("album", "Single")
        metadata["artist"] = video_metadata.get("artist",metadata["artist"])

    return metadata



def download(url,title,mp3):
    ydl_opts = {
        'format': 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/mp4',
        'outtmpl': title+'.mp4',  
    }

    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        ydl.download([url])
        if(mp3):
            convert = VideoFileClip(title+".mp4")
            convert.audio.write_audiofile(title+".mp3")
            convert.close()
            delete_file(title+".mp4")



def save_metadata(metadata):
    with open(metadata["title"]+".jpg", "wb") as file:
        response = requests.get(metadata["miniature_url"])
        file.write(response.content)
        file.close()

    audiofile = eyed3.load(metadata["title"]+".mp3")
    if (audiofile.tag == None):
        audiofile.initTag()

    audiofile.tag.images.set(ImageFrame.FRONT_COVER, open(metadata["title"]+'.jpg','rb').read(), 'image/jpeg')
    audiofile.tag.title = metadata["title"]
    audiofile.tag.artist = metadata["artist"]
    audiofile.tag.album = metadata["album"]

    audiofile.tag.save()

    delete_file(metadata["title"]+".jpg")


def download_video(url,already_download,mp3):
    try :
        title = replace_slash(YouTube(url).title)
    except :
        print("The url pass is not correct")
        return
    
    print("title:", title)
    for file in already_download:
        if((mp3 and file==title+".mp3") or file==title+".mp4"):
            print("Already Download")
            return
        
    if(mp3):
        print("Recover metadata")
        metadata = recover_metadata(url)
    
    print("Download :")
    download(url,title,mp3)

    if(mp3):
        print("Save the metadata")
        save_metadata(metadata)


def download_playlist(url,already_download,mp3):
    playlist = Playlist(url)
    for i in range(len(playlist)):
        print("Processing :"+str(i+1)+"/"+str(len(playlist)))
        download_video(playlist[i],already_download,mp3)


def download_list(file,already_download,mp3,playlist):
    with open(file, "r") as urls:
        for url in urls: 
            if(playlist):
                download_playlist(url,already_download,mp3)
            else:
                download_video(url,already_download,mp3)



def main():
    already_download = os.listdir(".")

    parser = argparse.ArgumentParser(description="Take a youtube url and download the video in mp4")
    parser.add_argument("url", nargs='?', help="URL to be processed")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-mp3", action="store_true", help="Option to download only the audio")  
    group.add_argument("-p", "--playlist", action="store_true", help="Option to download all videos of a playlist")     
    group.add_argument("-l", "--list", action="store_true", help="Option to download a list of video, pass a file with an url by line")    
    args = parser.parse_args()

    mp3 = False
    if args.mp3:
        mp3 = True    
    elif args.list and args.playlist :
        download_list(args.url,already_download,mp3,True)
    elif args.list :
        download_list(args.url,already_download,mp3,False)
    elif args.playlist :
        download_playlist(args.url,already_download,mp3)
    else:
        download_video(args.url,already_download,mp3)    

if __name__ == "__main__":
    main()

