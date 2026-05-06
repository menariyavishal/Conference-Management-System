import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import type { Variants } from 'framer-motion';
import { FileText, PlusCircle, Clock, CheckCircle, XCircle, Loader, ChevronRight } from 'lucide-react';
import { useAuth } from '../../context/AuthContext';
import submissionService from '../../services/submission.service';
import type { Submission } from '../../types/api';
import NewSubmissionModal from './NewSubmissionModal';

const AuthorDashboard: React.FC = () => {
  const { user } = useAuth();
  const [submissions, setSubmissions] = useState<Submission[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [stats, setStats] = useState({ total: 0, pending: 0, accepted: 0, rejected: 0 });
  
  const fetchData = async () => {
    try {
      setIsLoading(true);
      const response = await submissionService.list();
      if (response.success) {
        const subs = response.data;
        setSubmissions(subs);
        
        // Calculate stats
        const newStats = {
          total: subs.length,
          pending: subs.filter(s => s.status === 'PENDING' || s.status === 'UNDER_REVIEW').length,
          accepted: subs.filter(s => s.status === 'ACCEPTED').length,
          rejected: subs.filter(s => s.status === 'REJECTED').length
        };
        setStats(newStats);
      }
    } catch (error) {
      console.error("Error fetching submissions:", error);
    } finally {
      setIsLoading(false);
    }
  };

  useEffect(() => {
    fetchData();
  }, []);

  // Animation variants
  const containerVariants: Variants = {
    hidden: { opacity: 0 },
    show: {
      opacity: 1,
      transition: { staggerChildren: 0.1 }
    }
  };
  
  const itemVariants: Variants = {
    hidden: { opacity: 0, y: 30 },
    show: { opacity: 1, y: 0, transition: { type: "spring", stiffness: 260, damping: 20 } }
  };

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'ACCEPTED': return 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20';
      case 'REJECTED': return 'bg-rose-500/10 text-rose-400 border-rose-500/20';
      case 'UNDER_REVIEW': return 'bg-amber-500/10 text-amber-400 border-amber-500/20';
      default: return 'bg-blue-500/10 text-blue-400 border-blue-500/20';
    }
  };

  return (
    <motion.div 
      variants={containerVariants}
      initial="hidden"
      animate="show"
      className="space-y-10"
    >
      {/* Welcome Banner */}
      <motion.div variants={itemVariants} className="relative rounded-[2.5rem] overflow-hidden shadow-2xl group">
        <div className="absolute inset-0">
          <img 
            src="https://images.unsplash.com/photo-1540575467063-178a50c2df87?auto=format&fit=crop&w=1200&q=80" 
            alt="Conference Hall" 
            className="w-full h-full object-cover opacity-30 mix-blend-overlay transition-transform duration-700 group-hover:scale-105"
          />
          <div className="absolute inset-0 bg-gradient-to-br from-blue-600/20 via-[#0a0f1e]/80 to-[#0a0f1e]" />
        </div>
        <div className="relative px-10 py-16 sm:px-16 sm:py-20">
          <motion.div
            initial={{ opacity: 0, x: -20 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ delay: 0.2 }}
          >
            <h1 className="text-4xl sm:text-5xl font-black tracking-tight text-white mb-4">
              Hello, <span className="text-transparent bg-clip-text bg-gradient-to-r from-blue-400 to-indigo-400">{user?.fullName?.split(' ')[0]}</span>
            </h1>
            <p className="text-xl text-slate-400 max-w-xl font-medium leading-relaxed">
              Your research hub is ready. Track your paper status, explore feedback, and prepare for your next big presentation.
            </p>
            <div className="mt-10 flex flex-wrap gap-5">
              <motion.button 
                onClick={() => setIsModalOpen(true)}
                whileHover={{ scale: 1.02, translateY: -2 }}
                whileTap={{ scale: 0.98 }}
                className="inline-flex items-center px-8 py-4 bg-blue-600 hover:bg-blue-500 text-white font-bold rounded-2xl shadow-xl shadow-blue-600/20 transition-all"
              >
                <PlusCircle className="w-5 h-5 mr-3" />
                New Submission
              </motion.button>
              <motion.button 
                whileHover={{ scale: 1.02, translateY: -2 }}
                whileTap={{ scale: 0.98 }}
                className="inline-flex items-center px-8 py-4 bg-white/5 hover:bg-white/10 text-white font-bold rounded-2xl border border-white/10 backdrop-blur-md transition-all"
              >
                <FileText className="w-5 h-5 mr-3" />
                Guidelines
              </motion.button>
            </div>
          </motion.div>
        </div>
      </motion.div>

      {/* Stats Cards Grid */}
      <div className="grid grid-cols-1 gap-6 sm:grid-cols-2 lg:grid-cols-4">
        {[
          { label: 'Submissions', value: stats.total, icon: FileText, color: 'text-blue-400', bg: 'bg-blue-400/10', border: 'border-blue-400/20' },
          { label: 'Under Review', value: stats.pending, icon: Clock, color: 'text-amber-400', bg: 'bg-amber-400/10', border: 'border-amber-400/20' },
          { label: 'Accepted', value: stats.accepted, icon: CheckCircle, color: 'text-emerald-400', bg: 'bg-emerald-400/10', border: 'border-emerald-400/20' },
          { label: 'Rejected', value: stats.rejected, icon: XCircle, color: 'text-rose-400', bg: 'bg-rose-400/10', border: 'border-rose-400/20' }
        ].map((stat, idx) => (
          <motion.div 
            key={idx}
            variants={itemVariants}
            whileHover={{ y: -8, transition: { duration: 0.2 } }}
            className={`bg-white/5 backdrop-blur-xl border ${stat.border} rounded-3xl p-7 shadow-xl group transition-all`}
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-xs font-black text-slate-500 uppercase tracking-[0.2em] mb-1">{stat.label}</p>
                <p className="text-4xl font-black text-white">{stat.value}</p>
              </div>
              <div className={`p-4 rounded-2xl ${stat.bg} group-hover:scale-110 transition-transform duration-300`}>
                <stat.icon className={`w-7 h-7 ${stat.color}`} />
              </div>
            </div>
          </motion.div>
        ))}
      </div>

      {/* Table Section */}
      <motion.div variants={itemVariants} className="bg-white/5 backdrop-blur-xl border border-white/10 rounded-[2rem] shadow-2xl overflow-hidden">
        <div className="px-10 py-8 border-b border-white/5 flex justify-between items-center">
          <div>
            <h3 className="text-2xl font-bold text-white">Recent Activity</h3>
            <p className="text-slate-500 text-sm mt-1">Status of your latest research submissions.</p>
          </div>
          <button className="flex items-center px-4 py-2 text-sm font-bold text-blue-400 hover:text-blue-300 bg-blue-400/5 hover:bg-blue-400/10 rounded-xl transition-all">
            History <ChevronRight className="w-4 h-4 ml-1" />
          </button>
        </div>
        
        {isLoading ? (
          <div className="py-32 flex flex-col items-center justify-center">
            <Loader className="w-12 h-12 text-blue-500 animate-spin mb-6" />
            <p className="text-slate-400 font-medium tracking-wide">Retrieving your manuscripts...</p>
          </div>
        ) : submissions.length > 0 ? (
          <div className="overflow-x-auto">
            <table className="w-full text-left">
              <thead>
                <tr className="bg-white/5">
                  <th className="px-10 py-5 text-xs font-black text-slate-500 uppercase tracking-widest">Manuscript</th>
                  <th className="px-10 py-5 text-xs font-black text-slate-500 uppercase tracking-widest">Status</th>
                  <th className="px-10 py-5 text-xs font-black text-slate-500 uppercase tracking-widest">Submission Date</th>
                  <th className="px-10 py-5 text-xs font-black text-slate-500 uppercase tracking-widest text-right">Actions</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-white/5">
                {submissions.slice(0, 5).map((sub) => (
                  <tr key={sub.id || sub.submissionId} className="hover:bg-white/[0.03] transition-colors group">
                    <td className="px-10 py-7">
                      <div className="flex items-center">
                        <div className="w-12 h-12 rounded-xl bg-blue-500/10 flex items-center justify-center mr-4 group-hover:bg-blue-500/20 transition-colors">
                          <FileText className="w-6 h-6 text-blue-400" />
                        </div>
                        <div>
                          <p className="text-sm font-bold text-white mb-1 group-hover:text-blue-300 transition-colors">{sub.title}</p>
                          <p className="text-xs text-slate-500 font-medium">{sub.authors}</p>
                        </div>
                      </div>
                    </td>
                    <td className="px-10 py-7">
                      <span className={`inline-flex items-center px-4 py-1.5 rounded-full text-[10px] font-black uppercase tracking-widest border ${getStatusColor(sub.status)}`}>
                        {sub.status.replace('_', ' ')}
                      </span>
                    </td>
                    <td className="px-10 py-7">
                      <p className="text-sm font-semibold text-slate-300">{new Date(sub.createdAt).toLocaleDateString(undefined, { year: 'numeric', month: 'short', day: 'numeric' })}</p>
                    </td>
                    <td className="px-10 py-7 text-right">
                      <button className="px-6 py-2.5 bg-white/5 hover:bg-white/10 border border-white/10 text-white font-bold text-xs rounded-xl transition-all">
                        Review Details
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        ) : (
          <div className="py-24 text-center">
            <div className="inline-flex items-center justify-center w-24 h-24 rounded-3xl bg-slate-800/50 mb-6 border border-white/5">
              <FileText className="w-10 h-10 text-slate-600" />
            </div>
            <h3 className="text-xl font-bold text-white mb-2">No Submissions Found</h3>
            <p className="text-slate-500 mb-10 max-w-sm mx-auto">Start your conference journey by submitting your first research paper today.</p>
            <motion.button 
              onClick={() => setIsModalOpen(true)}
              whileHover={{ scale: 1.05 }}
              whileTap={{ scale: 0.95 }}
              className="inline-flex items-center px-8 py-4 bg-blue-600 hover:bg-blue-500 text-white font-bold rounded-2xl shadow-xl shadow-blue-600/20 transition-all"
            >
              <PlusCircle className="w-5 h-5 mr-3" />
              Create First Submission
            </motion.button>
          </div>
        )}
      </motion.div>

      <NewSubmissionModal 
        isOpen={isModalOpen}
        onClose={() => setIsModalOpen(false)}
        onSuccess={fetchData}
      />
    </motion.div>
  );
};

export default AuthorDashboard;
