# # https://huggingface.co/suriya7/bart-finetuned-text-summarization
# from transformers import AutoTokenizer, AutoModelForSeq2SeqLM

# tokenizer = AutoTokenizer.from_pretrained("/mnt/nvme1n1/cyl/suoju/suriya7/bart-finetuned-text-summarization")
# model = AutoModelForSeq2SeqLM.from_pretrained("/mnt/nvme1n1/cyl/suoju/suriya7/bart-finetuned-text-summarization")

# def generate_summary(text):
#     inputs = tokenizer([text], max_length=1024, return_tensors='pt', truncation=True)
#     summary_ids = model.generate(inputs['input_ids'], max_new_tokens=100, do_sample=False)
#     summary = tokenizer.decode(summary_ids[0], skip_special_tokens=True)
#     return summary
# with open('/mnt/nvme2n1/cyl/cyl/cLLama-CPU/llama.cpp/application.log', 'r') as f: 
#     file_data = f.read() 
# text_to_summarize = file_data 
# print(text_to_summarize)
# # text_to_summarize = """Now, there is no doubt that one of the most important aspects of any Pixel phone is its camera.And there might be good news for all camera lovers. Rumours have suggested that the Pixel 9 could come with a telephoto lens,improving its photography capabilities even further. Google will likely continue to focus on using AI to enhance its camera performance,in order to make sure that Pixel phones remain top contenders in the world of mobile photography."""
# summary = generate_summary(text_to_summarize)
# print(summary)
from transformers import pipeline

def suoju(a):
    summarizer = pipeline("summarization", model="/mnt/nvme1n1/cyl/facebook/bart-large-cnn", batch_size=20,device=-1)

    with open('/mnt/nvme2n1/cyl/cyl/cLLama-CPU/llama.cpp/application.log', 'r') as f: 
        file_data = f.read() 
    ARTICLE = file_data 
    if a==0:
        ARTICLE='test'
    re=(summarizer(ARTICLE, max_length=int(len(ARTICLE)*0.8)+1, min_length=int(len(ARTICLE)*0.5), do_sample=True))
    print(re)
    return re

suoju(0)