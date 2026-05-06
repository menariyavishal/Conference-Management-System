import React, { useState, useRef } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { X, Send, FileText, User, AlignLeft, Loader, Upload, Paperclip, CheckCircle2, Hash } from 'lucide-react';
import submissionService from '../../services/submission.service';
import { toast } from 'react-hot-toast';

interface NewSubmissionModalProps {
  isOpen: boolean;
  onClose: () => void;
  onSuccess: () => void;
}

const NewSubmissionModal: React.FC<NewSubmissionModalProps> = ({ isOpen, onClose, onSuccess }) => {
  const [formData, setFormData] = useState({
    title: '',
    abstract: '',
    authors: '',
    keywords: ''
  });
  const [file, setFile] = useState<File | null>(null);
  const [isSubmitting, setIsSubmitting] = useState(false);
  const fileInputRef = useRef<HTMLInputElement>(null);

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const selectedFile = e.target.files?.[0];
    if (selectedFile) {
      if (selectedFile.size > 10 * 1024 * 1024) {
        toast.error('File size exceeds 10MB limit');
        return;
      }
      const allowedTypes = ['application/pdf', 'application/msword', 'application/vnd.openxmlformats-officedocument.wordprocessingml.document'];
      if (!allowedTypes.includes(selectedFile.type)) {
        toast.error('Only PDF and Word documents are allowed');
        return;
      }
      setFile(selectedFile);
    }
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!formData.title || !formData.abstract || !formData.authors || !formData.keywords) {
      toast.error('Please fill in all text fields including keywords');
      return;
    }

    if (!file) {
      toast.error('Please upload your manuscript file');
      return;
    }

    try {
      setIsSubmitting(true);
      
      const data = new FormData();
      data.append('title', formData.title);
      data.append('abstract', formData.abstract);
      data.append('authors', formData.authors);
      data.append('keywords', formData.keywords);
      data.append('file', file);
      data.append('status', 'PENDING');

      const response = await submissionService.create(data);

      if (response.success) {
        toast.success('Manuscript submitted successfully!');
        onSuccess();
        onClose();
        setFormData({ title: '', abstract: '', authors: '', keywords: '' });
        setFile(null);
      } else {
        toast.error(response.message || 'Submission failed');
      }
    } catch (error: any) {
      toast.error(error.message || 'Failed to submit paper');
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <AnimatePresence>
      {isOpen && (
        <div className="fixed inset-0 z-[100] flex items-center justify-center p-4 sm:p-6 overflow-hidden">
          <motion.div
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            onClick={onClose}
            className="absolute inset-0 bg-[#020617]/90 backdrop-blur-md"
          />
          <motion.div
            initial={{ opacity: 0, scale: 0.9, y: 40 }}
            animate={{ opacity: 1, scale: 1, y: 0 }}
            exit={{ opacity: 0, scale: 0.9, y: 40 }}
            className="relative w-full max-w-3xl max-h-[90vh] flex flex-col bg-[#0f172a] border border-white/10 rounded-[2.5rem] shadow-[0_32px_64px_-12px_rgba(0,0,0,0.8)] overflow-hidden"
          >
            {/* Header */}
            <div className="flex-none px-10 py-8 border-b border-white/5 flex justify-between items-center bg-gradient-to-r from-blue-600/10 to-transparent">
              <div>
                <h2 className="text-3xl font-black text-white flex items-center tracking-tight">
                  <div className="p-3 rounded-2xl bg-blue-500/20 mr-4">
                    <Upload className="w-6 h-6 text-blue-400" />
                  </div>
                  Submit Manuscript
                </h2>
                <p className="text-slate-400 text-sm mt-2 font-medium">Upload your research paper and provide details.</p>
              </div>
              <button 
                onClick={onClose}
                className="p-3 rounded-2xl hover:bg-white/5 text-slate-500 hover:text-white transition-all transform hover:rotate-90"
              >
                <X className="w-6 h-6" />
              </button>
            </div>

            {/* Form - Scrollable */}
            <form 
              id="submission-form"
              onSubmit={handleSubmit} 
              className="flex-1 overflow-y-auto p-10 space-y-8 custom-scrollbar"
            >
              <div className="grid grid-cols-1 md:grid-cols-2 gap-8">
                <div className="space-y-2">
                  <label className="text-xs font-black text-slate-500 uppercase tracking-widest flex items-center">
                    <FileText className="w-3.5 h-3.5 mr-2 text-blue-500" />
                    Manuscript Title
                  </label>
                  <input
                    type="text"
                    required
                    value={formData.title}
                    onChange={(e) => setFormData({ ...formData, title: e.target.value })}
                    placeholder="The impact of AI..."
                    className="w-full bg-white/[0.03] border border-white/10 rounded-2xl px-6 py-4 text-white placeholder-slate-600 focus:border-blue-500/50 focus:ring-4 focus:ring-blue-500/10 outline-none transition-all font-medium"
                  />
                </div>

                <div className="space-y-2">
                  <label className="text-xs font-black text-slate-500 uppercase tracking-widest flex items-center">
                    <User className="w-3.5 h-3.5 mr-2 text-blue-500" />
                    Co-Authors
                  </label>
                  <input
                    type="text"
                    required
                    value={formData.authors}
                    onChange={(e) => setFormData({ ...formData, authors: e.target.value })}
                    placeholder="e.g. Dr. John Smith"
                    className="w-full bg-white/[0.03] border border-white/10 rounded-2xl px-6 py-4 text-white placeholder-slate-600 focus:border-blue-500/50 focus:ring-4 focus:ring-blue-500/10 outline-none transition-all font-medium"
                  />
                </div>
              </div>

              <div className="space-y-2">
                <label className="text-xs font-black text-slate-500 uppercase tracking-widest flex items-center">
                  <Hash className="w-3.5 h-3.5 mr-2 text-blue-500" />
                  Keywords (comma separated)
                </label>
                <input
                  type="text"
                  required
                  value={formData.keywords}
                  onChange={(e) => setFormData({ ...formData, keywords: e.target.value })}
                  placeholder="AI, Machine Learning, Research..."
                  className="w-full bg-white/[0.03] border border-white/10 rounded-2xl px-6 py-4 text-white placeholder-slate-600 focus:border-blue-500/50 focus:ring-4 focus:ring-blue-500/10 outline-none transition-all font-medium"
                />
              </div>

              <div className="space-y-2">
                <label className="text-xs font-black text-slate-500 uppercase tracking-widest flex items-center">
                  <AlignLeft className="w-3.5 h-3.5 mr-2 text-blue-500" />
                  Abstract
                </label>
                <textarea
                  required
                  rows={5}
                  value={formData.abstract}
                  onChange={(e) => setFormData({ ...formData, abstract: e.target.value })}
                  placeholder="Provide a detailed summary of your work..."
                  className="w-full bg-white/[0.03] border border-white/10 rounded-2xl px-6 py-4 text-white placeholder-slate-600 focus:border-blue-500/50 focus:ring-4 focus:ring-blue-500/10 outline-none transition-all font-medium resize-none leading-relaxed"
                />
              </div>

              {/* File Upload Section */}
              <div className="space-y-2">
                <label className="text-xs font-black text-slate-500 uppercase tracking-widest flex items-center">
                  <Paperclip className="w-3.5 h-3.5 mr-2 text-blue-500" />
                  Upload Document (PDF/DOCX)
                </label>
                <div 
                  onClick={() => fileInputRef.current?.click()}
                  className={`relative group cursor-pointer border-2 border-dashed rounded-3xl p-10 transition-all duration-300 flex flex-col items-center justify-center text-center ${
                    file ? 'border-emerald-500/50 bg-emerald-500/5' : 'border-white/10 bg-white/[0.02] hover:border-blue-500/50 hover:bg-blue-500/5'
                  }`}
                >
                  <input 
                    type="file"
                    ref={fileInputRef}
                    onChange={handleFileChange}
                    accept=".pdf,.doc,.docx"
                    className="hidden"
                  />
                  
                  {file ? (
                    <motion.div initial={{ scale: 0.8, opacity: 0 }} animate={{ scale: 1, opacity: 1 }} className="flex flex-col items-center">
                      <div className="w-16 h-16 rounded-2xl bg-emerald-500/20 flex items-center justify-center mb-4">
                        <CheckCircle2 className="w-8 h-8 text-emerald-400" />
                      </div>
                      <p className="text-emerald-400 font-bold mb-1">{file.name}</p>
                      <p className="text-slate-500 text-xs">{(file.size / (1024 * 1024)).toFixed(2)} MB • Click to replace</p>
                    </motion.div>
                  ) : (
                    <>
                      <div className="w-16 h-16 rounded-2xl bg-blue-500/10 flex items-center justify-center mb-4 group-hover:scale-110 transition-transform">
                        <Upload className="w-8 h-8 text-blue-400" />
                      </div>
                      <p className="text-slate-300 font-bold mb-1">Select your manuscript</p>
                      <p className="text-slate-500 text-xs">Max file size: 10MB • PDF, DOCX supported</p>
                    </>
                  )}
                </div>
              </div>
            </form>

            {/* Footer */}
            <div className="flex-none p-10 bg-white/[0.02] border-t border-white/5 flex gap-5">
              <button
                type="button"
                onClick={onClose}
                className="flex-1 px-8 py-4 rounded-2xl border border-white/10 text-slate-400 font-bold hover:bg-white/5 hover:text-white transition-all"
              >
                Discard
              </button>
              <button
                type="submit"
                form="submission-form"
                disabled={isSubmitting}
                className="flex-[2] bg-blue-600 hover:bg-blue-500 disabled:bg-blue-900 text-white font-black py-4 px-8 rounded-2xl shadow-2xl shadow-blue-600/30 flex items-center justify-center transition-all group"
              >
                {isSubmitting ? (
                  <Loader className="w-6 h-6 animate-spin" />
                ) : (
                  <>
                    <Send className="w-5 h-5 mr-3 group-hover:translate-x-1 group-hover:-translate-y-1 transition-transform" />
                    Submit Manuscript
                  </>
                )}
              </button>
            </div>
          </motion.div>
        </div>
      )}
    </AnimatePresence>
  );
};

export default NewSubmissionModal;
